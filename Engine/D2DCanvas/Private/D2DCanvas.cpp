#include <d2d1helper.h>
#include <stdexcept>
#include <d2d1_3.h>
#include <dwrite.h>
#include <dxgi.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "Assert.hpp"
#include "Canvas2D.hpp"
#include "D2DCanvas.hpp"
#include "HashUtil.hpp"
#include "LinearAlgebra.hpp"
#include "RHIDevice.hpp"
#include "RHISwapchain.hpp"
#include "RHITexture.hpp"
#include "RHIFWD.hpp"
#include "StringUtil.hpp"

namespace{
    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    D2D1_POINT_2F convert(Smol::Vec2 v){
        return {v.x, v.y};
    }

    D2D1_COLOR_F convert(Smol::Vec4 v){
        return {v.x, v.y, v.z, v.w};
    }

    struct StrokeStyleKey{
        bool screenSpaceWidth;
        std::vector<Smol::f32> dashPattern;
    };

    struct StrokeStyleKeyView{
        bool screenSpaceWidth;
        std::span<const Smol::f32> dashPattern;
    };

    struct StrokeStyleKeyHash{
        using is_transparent = void; // heterogeneous lookup 활성화

        static Smol::usize hashImpl(bool fixed, std::span<const Smol::f32> dashes) noexcept{
            Smol::usize h = 0;

            for(auto dash: dashes){
                h = Smol::hashAll(h, dash);
            }

            return h;
        }

        Smol::usize operator()(const StrokeStyleKey& k) const noexcept{
            return hashImpl(k.screenSpaceWidth, k.dashPattern);
        }
        Smol::usize operator()(const StrokeStyleKeyView& k) const noexcept{
            return hashImpl(k.screenSpaceWidth, k.dashPattern);
        }
    };

    struct StrokeStyleKeyEqual{
        using is_transparent = void;

        template<class A, class B>
        bool operator()(const A& a, const B& b) const noexcept{
            // Notice. hash is based on bit pattern
            return a.screenSpaceWidth == b.screenSpaceWidth &&
                std::ranges::equal(
                    a.dashPattern, b.dashPattern,
                    [](Smol::f32 x, Smol::f32 y){
                        return std::bit_cast<Smol::u32>(x) == std::bit_cast<Smol::u32>(y);
                    }
                );
        }
    };

    struct TextStyleHash{
        using is_transparent = void;

        Smol::usize operator()(const Smol::TextStyle& k) const noexcept{
            // Only fontSize is needed for create TextFormat
            return Smol::hashAll(k.fontSize);
        }
    };

    struct TextStyleEqual{
        using is_transparent = void;

        template<class A, class B>
        bool operator()(const A& a, const B& b) const noexcept{
            return a.fontSize == b.fontSize;
        }
    };
}

namespace Smol
{
    Canvas2DRAII CreateD2DCanvas(RHIDevice& device){
        return std::make_unique<D2DCanvas>(device);
    }

    class D2DCanvas::Impl{
    private:
        ComPtr<ID2D1Factory8> factory = nullptr;
        ComPtr<ID2D1Device7> device = nullptr;
        ComPtr<ID2D1DeviceContext7> context = nullptr;

        ComPtr<IDWriteFactory> writeFactory = nullptr;
        std::unordered_map<
            StrokeStyleKey,
            ComPtr<ID2D1StrokeStyle1>,
            StrokeStyleKeyHash,
            StrokeStyleKeyEqual
        > strokeStyles;
        std::unordered_map<
            TextStyle,
            ComPtr<IDWriteTextFormat>,
            TextStyleHash,
            TextStyleEqual
        > textFormats;
        ComPtr<ID2D1SolidColorBrush> solidBrush = nullptr;

        ComPtr<ID2D1Bitmap1> target = nullptr;

    public:
        Impl(ID3D11Device&);

        void Begin(ID3D11Texture2D* texture);
        void End();

        void SetTransform(D2D1::Matrix3x2F transform);

        void Line(D2D1_POINT_2F v0, D2D1_POINT_2F v1, const StrokeStyle&);
        void Polyline(std::span<const Vec2>, const StrokeStyle&);
        void Circle(D2D1_POINT_2F center, f32 r, const StrokeStyle&);
        void Rect(D2D1_POINT_2F min, D2D1_POINT_2F max, const StrokeStyle&);
        void Text(StrView utf8, D2D1_POINT_2F pos, const TextStyle&);

    private:
        ID2D1StrokeStyle1* getOrCreateStrokeStyle(const StrokeStyle& s);
        IDWriteTextFormat* getOrCreateTextureFormat(const TextStyle& s);
    };

    D2DCanvas::D2DCanvas(RHIDevice& device)
        : impl(std::make_unique<Impl>(
            *static_cast<ID3D11Device*>(device.Get())
        ))
    {}

    D2DCanvas::Impl::Impl(ID3D11Device& d3dDevice){
        D2D1_FACTORY_OPTIONS opts{};
    #if defined(_DEBUG) || !defined(NDEBUG)
        opts.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    #endif

        if(FAILED(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            __uuidof(ID2D1Factory8),
            &opts,
            reinterpret_cast<void**>(factory.GetAddressOf())
        ))){
            throw std::runtime_error("Failed to create D2D Factory");
        }

        ComPtr<IDXGIDevice> dxgiDevice;
        d3dDevice.QueryInterface(IID_PPV_ARGS(&dxgiDevice));

        if(FAILED(factory->CreateDevice(
            dxgiDevice.Get(),
            device.GetAddressOf()
        ))){
            throw std::runtime_error("Failed to create D2D Device");
        }

        if(FAILED(device->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            context.GetAddressOf()
        ))){
            throw std::runtime_error("Failed to create D2D DeviceContext");
        }

        if(FAILED(context->CreateSolidColorBrush(
            convert(Colors::White),
            solidBrush.GetAddressOf()
        ))){
            throw std::runtime_error("Failed to create D2D Brush");
        }
    }

    D2DCanvas::~D2DCanvas() = default;

    void D2DCanvas::Begin(RHISwapchain& swapchain){
        auto texture = static_cast<ID3D11Texture2D*>(
            swapchain.GetCurrentTexture().GetNative()
        );
        impl->Begin(texture);
    }

    void D2DCanvas::Impl::Begin(ID3D11Texture2D* texture){
        SMOL_ASSERT(target == nullptr);

        ComPtr<IDXGISurface> surface;
        texture->QueryInterface(IID_PPV_ARGS(&surface));

        const auto props = D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED)
        );

        if(FAILED(context->CreateBitmapFromDxgiSurface(
            surface.Get(),
            &props,
            target.GetAddressOf()
        ))){
            throw std::runtime_error("Fai;ed create Bitmap from Surface");
        }

        context->SetTarget(target.Get());
        context->BeginDraw();
        context->SetTransform(D2D1::Matrix3x2F::Identity());

        if(FAILED(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(writeFactory.GetAddressOf())
        ))){
            throw std::runtime_error("Failed to create Write Factory");
        }
    }

    void D2DCanvas::End(){
        impl->End();
    }

    void D2DCanvas::Impl::End(){
        SMOL_ASSERT(target != nullptr);

        const auto hr = context->EndDraw();
        context->SetTarget(nullptr);
        target.Reset();

        if(hr == D2DERR_RECREATE_TARGET){
            throw std::runtime_error("D2D target lost (D3D Device Removed?)");
        }
    }

    void D2DCanvas::SetTransform(Transform2D transform){
        impl->SetTransform(
            D2D1::Matrix3x2F::Scale(transform.scale.x, transform.scale.y) *
            D2D1::Matrix3x2F::Rotation(static_cast<f32>(toDegree(transform.theta))) *
            D2D1::Matrix3x2F::Translation(transform.position.x, transform.position.y)
        );
    }

    void D2DCanvas::Impl::SetTransform(D2D1::Matrix3x2F transform){
        context->SetTransform(transform);
    }

    void D2DCanvas::Line(Vec2 v0, Vec2 v1, const StrokeStyle& style){
        impl->Line(convert(v0), convert(v1), style);
    }
    void D2DCanvas::Impl::Line(D2D1_POINT_2F v0, D2D1_POINT_2F v1, const StrokeStyle& style){
        solidBrush->SetColor(convert(style.color));

        context->DrawLine(
            v0,
            v1,
            solidBrush.Get(),
            style.width,
            getOrCreateStrokeStyle(style)
        );
    }

    void D2DCanvas::Polyline(std::span<const Vec2> points, const StrokeStyle& style){
        SMOL_ASSERT(points.size() >= 2);
        impl->Polyline(points, style);
    }
    void D2DCanvas::Impl::Polyline(std::span<const Vec2> points, const StrokeStyle& style){
        ComPtr<ID2D1PathGeometry> geo;
        if(FAILED(factory->CreatePathGeometry(&geo))){
            throw std::runtime_error("Failed to create Path Geometry");
        }

        ComPtr<ID2D1GeometrySink> sink;
        geo->Open(&sink);

        sink->BeginFigure(
            convert(points[0]),
            D2D1_FIGURE_BEGIN_HOLLOW
        );
        sink->AddLines(
            reinterpret_cast<const D2D1_POINT_2F*>(points.data() + 1),
            static_cast<UINT32>(points.size() - 1)
        );
        sink->EndFigure(D2D1_FIGURE_END_OPEN);

        sink->Close();

        solidBrush->SetColor(convert(style.color));

        context->DrawGeometry(
            geo.Get(),
            solidBrush.Get(),
            style.width,
            getOrCreateStrokeStyle(style)
        );
    }

    void D2DCanvas::Circle(Vec2 center, f32 r, const StrokeStyle& style){
        impl->Circle(convert(center), r, style);
    }
    void D2DCanvas::Impl::Circle(D2D1_POINT_2F center, f32 r, const StrokeStyle& style){
        solidBrush->SetColor(convert(style.color));

        context->DrawEllipse(
            D2D1::Ellipse(center, r, r),
            solidBrush.Get(),
            style.width,
            getOrCreateStrokeStyle(style)
        );
    }

    void D2DCanvas::Rect(Vec2 min, Vec2 max, const StrokeStyle& style){
        impl->Rect(convert(min), convert(max), style);
    }
    void D2DCanvas::Impl::Rect(D2D1_POINT_2F min, D2D1_POINT_2F max, const StrokeStyle& style){
        solidBrush->SetColor(convert(style.color));

        context->DrawRectangle(
            D2D1::Rect(min.x, min.y, max.x, max.y),
            solidBrush.Get(),
            style.width,
            getOrCreateStrokeStyle(style)
        );
    }

    void D2DCanvas::Text(StrView utf8, Vec2 pos, const TextStyle& style){
        impl->Text(utf8, convert(pos), style);
    }
    void D2DCanvas::Impl::Text(StrView utf8, D2D1_POINT_2F pos, const TextStyle& style){
        auto utf16Str = toUTF16String(utf8);
        solidBrush->SetColor(convert(style.color));

        context->DrawText(
            utf16Str.data(),
            static_cast<UINT32>(utf16Str.length()),
            getOrCreateTextureFormat(style),
            D2D1::RectF(
                pos.x,
                pos.y,
                pos.x + style.size.x,
                pos.y + style.size.y
            ),
            solidBrush.Get(),
            D2D1_DRAW_TEXT_OPTIONS_NONE,
            DWRITE_MEASURING_MODE_NATURAL
        );
    }

    ID2D1StrokeStyle1* D2DCanvas::Impl::getOrCreateStrokeStyle(const StrokeStyle& s){
        const StrokeStyleKeyView view{
            .screenSpaceWidth = s.screenSpaceWidth,
            .dashPattern = s.dashPattern
        };
        if(auto it = strokeStyles.find(view); it != strokeStyles.end())
            return it->second.Get();

        const auto props = D2D1::StrokeStyleProperties1(
            D2D1_CAP_STYLE_ROUND,
            D2D1_CAP_STYLE_ROUND,
            D2D1_CAP_STYLE_ROUND,
            D2D1_LINE_JOIN_ROUND,
            10.f,
            s.dashPattern.empty() ?
                D2D1_DASH_STYLE_SOLID :
                D2D1_DASH_STYLE_CUSTOM,
            0.f, // dashOffset
            s.screenSpaceWidth ?
                D2D1_STROKE_TRANSFORM_TYPE_FIXED :
                D2D1_STROKE_TRANSFORM_TYPE_NORMAL
        );

        ComPtr<ID2D1StrokeStyle1> style;
        if(FAILED(factory->CreateStrokeStyle(
            &props,
            s.dashPattern.data(),
            static_cast<UINT32>(s.dashPattern.size()),
            &style
        ))){
            throw std::runtime_error("Failed to create Stroke style");
        }

        auto [it, ret] = strokeStyles.emplace(
            StrokeStyleKey{
                .screenSpaceWidth = s.screenSpaceWidth,
                .dashPattern = {s.dashPattern.begin(), s.dashPattern.end()}
            },
            std::move(style)
        );
        SMOL_ASSERT(ret);

        return it->second.Get();
    }

    IDWriteTextFormat* D2DCanvas::Impl::getOrCreateTextureFormat(const TextStyle& s){
        if(auto it = textFormats.find(s); it != textFormats.end())
            return it->second.Get();

        ComPtr<IDWriteTextFormat> textFormat;
        if(FAILED(writeFactory->CreateTextFormat(
            // TODO
            L"Cascadia Mono",
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            s.fontSize,
            L"",
            textFormat.GetAddressOf()
        ))){
            throw std::runtime_error("Failed to create Text Format");
        }

        textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
        textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
        textFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

        auto [it, ret] = textFormats.emplace(
            s,
            std::move(textFormat)
        );
        SMOL_ASSERT(ret);

        return it->second.Get();
    }
}
