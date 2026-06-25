#pragma once

#include <filesystem>
#include <limits>
#include <optional>
#include <span>
#include <utility>
#include <variant>
#include "EnumUtil.hpp"
#include "HashUtil.hpp"
#include "Primitives.hpp"
#include "StringUtil.hpp"

namespace Smol
{
    struct RHICapabilities{
        bool flipTextureV;
        f32 clipSpaceMinZ;
    };

    enum class RHIMemoryAccess: u8{
        GPUOnly  = 0,
        CPUWrite = 1,
        CPURead  = 2
    };

    enum class RHIBufferUsage: u16{
        None             = 0,
        // fixed binding
        VertexBuffer     = 1 << 0,
        IndexBuffer      = 1 << 1,
        ConstantBuffer   = 1 << 2,
        // view capability
        AllowShaderRead  = 1 << 4,
        AllowShaderWrite = 1 << 5,
        // others
        IndirectArgs     = 1 << 6,
        CopySource       = 1 << 7,
        CopyDest         = 1 << 8
    };

    constexpr auto BUF_AllowShaderRW = combine(
        RHIBufferUsage::AllowShaderRead,
        RHIBufferUsage::AllowShaderWrite
    );

    struct RHIBufferCreateDesc{
        u32 size;
        RHIBufferUsage usage = RHIBufferUsage::None;
        RHIMemoryAccess access = RHIMemoryAccess::GPUOnly;
        const void* initialData = nullptr;
    };

    enum class RHIPrimitiveTopology: u8{
        PointList     = 0,
        LineList      = 1,
        LineStrip     = 2,
        TriangleList  = 3,
        TriangleStrip = 4,
    };

    enum class RHIPixelFormat: u8{
        Unknown = 0,

        // 8-bit formats
        R8_UNORM,
        R8_SNORM,
        R8_UINT,
        R8_SINT,

        // 16-bit formats
        R16_UNORM,
        R16_SNORM,
        R16_UINT,
        R16_SINT,
        R16_FLOAT,

        RG8_UNORM,
        RG8_SNORM,
        RG8_UINT,
        RG8_SINT,

        // 32-bit formats
        R32_UINT,
        R32_SINT,
        R32_FLOAT,

        RG16_UNORM,
        RG16_SNORM,
        RG16_UINT,
        RG16_SINT,
        RG16_FLOAT,

        RGBA8_UNORM,
        RGBA8_UNORM_SRGB,
        RGBA8_SNORM,
        RGBA8_UINT,
        RGBA8_SINT,

        BGRA8_UNORM,
        BGRA8_UNORM_SRGB,

        // 64-bit formats
        RG32_UINT,
        RG32_SINT,
        RG32_FLOAT,

        // 96-bit formats
        RGB32_FLOAT,

        RGBA16_UNORM,
        RGBA16_SNORM,
        RGBA16_UINT,
        RGBA16_SINT,
        RGBA16_FLOAT,

        // 128-bit formats
        RGBA32_UINT,
        RGBA32_SINT,
        RGBA32_FLOAT,

        // Depth/stencil formats
        D16_UNORM,
        D24_UNORM_S8_UINT,
        D32_FLOAT,
        D32_FLOAT_S8_UINT,

        // Compressed formats
        BC1_UNORM,
        BC1_UNORM_SRGB,
        BC2_UNORM,
        BC2_UNORM_SRGB,
        BC3_UNORM,
        BC3_UNORM_SRGB,
        BC4_UNORM,
        BC4_SNORM,
        BC5_UNORM,
        BC5_SNORM,
        BC6H_UF16,
        BC6H_SF16,
        BC7_UNORM,
        BC7_UNORM_SRGB,
    };

    enum class RHITextureUsage: u8{
        None              = 0,
        AllowShaderRead   = 1 << 0,
        AllowRenderTarget = 1 << 1,
        AllowDepthStencil = 1 << 2,
        AllowShaderWrite  = 1 << 3,
        CopySource        = 1 << 4,
        CopyDest          = 1 << 5,
    };

    constexpr auto TEX_AllowShaderRW = combine(
        RHITextureUsage::AllowShaderRead,
        RHITextureUsage::AllowShaderWrite
    );

    enum class RHIResourceState: u16{
        Common                    = 0,
        VertexAndConstantBuffer   = 1 << 0,
        IndexBuffer               = 1 << 1,
        RenderTarget              = 1 << 2,
        UnorderedAccess           = 1 << 3,
        DepthWrite                = 1 << 4,
        DepthRead                 = 1 << 5,
        NonFragmentShaderResource = 1 << 6,
        FragmentShaderResource    = 1 << 7,
        StreamOut                 = 1 << 8,
        IndirectArgument          = 1 << 9,
        CopyDest                  = 1 << 10,
        CopySource                = 1 << 11,
        ResolveDest               = 1 << 12,
        ResolveSource             = 1 << 13,
        GenericRead               =(1 << 14),
        AllShaderResource,
        Present,
        Predication
    };

    struct RHIClearDepthStencil{
        f32 depth = 1.0f;
        u8 stencil = 0;
    };

    struct RHITextureCreateDesc{
        // 0 for same as screen
        u32 width = 0, height = 0;
        u32 depth = 1;
        u32 mipLevels = 1;
        u32 arraySize = 1;
        RHIPixelFormat format = RHIPixelFormat::BGRA8_UNORM;
        RHITextureUsage usage = RHITextureUsage::None;
        RHIMemoryAccess access = RHIMemoryAccess::GPUOnly;
        RHIResourceState initialState = RHIResourceState::Common;
        Color clearColor = Colors::Black;
        RHIClearDepthStencil clearDepthStencil{};
        const void* initialData = nullptr;
    };

    enum class RHIShaderStage: u8{
        VertexShader,
        FragmentShader,
        ComputeShader,
    };

    enum class RHILoadAction: u8{
        Load,    // Preserve existing contents
        Clear,   // Clear to specified color
        DontCare // Don't care about existing contents
    };

    enum class RHIStoreAction: u8{
        Store,    // Save contents
        DontCare, // Don't care about existing contents
    };

    enum class RHIIndexFormat{
        UInt16,
        UInt32,
    };

    struct RHIViewport{
        f32 x, y;
        f32 width, height;
        f32 minDepth, maxDepth;
    };

    struct RHIScissorRect{
        i32 left, top;
        i32 right, bottom;
    };

    enum class RHIInputClassification: u8{
        PerVertex,
        PerInstance,
    };

    struct RHIVertexElement{
        CStr semanticName = nullptr;
        u32 semanticIndex;
        RHIPixelFormat format;
        u32 inputSlot;
        u32 alignedByteOffset;
        RHIInputClassification classification;
        u32 instanceDataStepRate; // For per-instance data
    };

    enum class RHICullMode: u8{
        CullNone,
        Front,
        Back,
    };

    enum class RHIFillMode: u8{
        Solid,
        Wireframe,
    };

    struct RHIRasterizerState{
        RHIFillMode fillMode = RHIFillMode::Solid;
        RHICullMode cullMode = RHICullMode::Back;
        bool frontCounterClockwise = true;
        i32 depthBias = 0;
        f32 depthBiasClamp = 0.0f;
        f32 slopeScaledDepthBias = 0.0f;
        bool depthClipEnable = true;
        bool multisampleEnable = false;
        bool antialiasedLineEnable = false;
    };

    enum class RHIComparisonFunc: u8{
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always,
    };

    enum class RHIStencilOp: u8{
        Keep,
        Zero,
        Replace,
        IncrSat,
        DecrSat,
        Invert,
        IncrWrap,
        DecrWrap,
    };

    struct RHIStencilOpDesc{
        RHIComparisonFunc func = RHIComparisonFunc::Always;
        RHIStencilOp stencilFailOp = RHIStencilOp::Keep;
        RHIStencilOp depthFailOp = RHIStencilOp::Keep;
        RHIStencilOp passOp = RHIStencilOp::Keep;
    };

    struct RHIStencilState{
        u8 readMask = 0xFF;
        u8 writeMask = 0xFF;
        RHIStencilOpDesc frontFace = {};
        RHIStencilOpDesc backFace = {};
    };

    struct RHIDepthStencilState{
        RHIPixelFormat format = RHIPixelFormat::D32_FLOAT;
        bool depthWriteEnable = false;
        RHIComparisonFunc depthFunc = RHIComparisonFunc::Less;
        std::optional<RHIStencilState> stencil = std::nullopt;
    };

    enum class RHIBlend: u8{
        Zero,
        One,
        SrcColor,
        InvSrcColor,
        SrcAlpha,
        InvSrcAlpha,
        DestAlpha,
        InvDestAlpha,
        DestColor,
        InvDestColor,
        SrcAlphaSat,
        BlendFactor,
        InvBlendFactor,
    };

    enum class RHIBlendOp: u8{
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max,
    };

    enum class RHIColorWriteMask: u8{
        EnableRed   = 1 << 0,
        EnableGreen = 1 << 1,
        EnableBlue  = 1 << 2,
        EnableAlpha = 1 << 3
    };
    constexpr auto RHIColorWriteEnableColor = combine(
        RHIColorWriteMask::EnableRed,
        RHIColorWriteMask::EnableGreen,
        RHIColorWriteMask::EnableBlue
    );
    constexpr auto RHIColorWriteEnableAll = combine(
        RHIColorWriteEnableColor,
        RHIColorWriteMask::EnableAlpha
    );

    struct RHIRenderTargetBlendState{
        bool blendEnable = false;
        RHIBlend srcBlend = RHIBlend::One;
        RHIBlend dstBlend = RHIBlend::Zero;
        RHIBlendOp blendOp = RHIBlendOp::Add;
        RHIBlend srcBlendAlpha = RHIBlend::One;
        RHIBlend dstBlendAlpha = RHIBlend::Zero;
        RHIBlendOp blendOpAlpha = RHIBlendOp::Add;
        RHIColorWriteMask writeMask = RHIColorWriteEnableAll;
    };

    struct RHIBlendState{
        bool alphaToCoverageEnable = false;
        bool independentBlendEnable = false;
        RHIRenderTargetBlendState renderTargets[8];
    };

    constexpr u32 RHI_MAX_RENDER_TARGETS = 8;

    struct RHIGraphicsPipelineStateDesc{
        std::optional<std::span<const RHIVertexElement>> vertexLayout = std::nullopt;
        RHIPrimitiveTopology topology = RHIPrimitiveTopology::TriangleList;

        std::filesystem::path vertexShaderPath;
        Str vertexShaderEntryPoint = "vs_main";
        RHIRasterizerState rasterizer = {};
        std::filesystem::path fragmentShaderPath;
        Str fragmentShaderEntryPoint = "fs_main";

        std::optional<RHIDepthStencilState> depthStencil = std::nullopt;
        std::optional<RHIBlendState> blend = std::nullopt;

        RHIPixelFormat renderTargetFormats[RHI_MAX_RENDER_TARGETS] = {};
        u32 renderTargetCount = 1;
    };

    struct RHISize3D{
        u32 x = 1, y = 1, z = 1;
    };

    inline bool operator==(const RHISize3D& lhs, const RHISize3D& rhs) noexcept{
        return lhs.x==rhs.x && lhs.y==rhs.y && lhs.z==rhs.z;
    }
    inline bool operator<=(const RHISize3D& lhs, const RHISize3D& rhs) noexcept{
        return lhs.x<=rhs.x && lhs.y<=rhs.y && lhs.z<=rhs.z;
    }

    struct RHIComputePipelineStateDesc{
        std::filesystem::path computeShaderPath;
        Str computeShaderEntryPoint = "cs_main";
        RHISize3D gridSize;
        std::optional<RHISize3D> threadGroupSize = std::nullopt;
    };

    enum class RHIFilter: u8{
        Nearest,
        Linear
    };

    enum class RHIAddressMode: u8{
        Wrap,
        Clamp,
        Mirror,
        Border
    };

    struct RHISamplerState{
        RHIFilter minFilter = RHIFilter::Linear;
        RHIFilter magFilter = RHIFilter::Linear;
        RHIFilter mipFilter = RHIFilter::Linear;

        RHIAddressMode addressU = RHIAddressMode::Wrap;
        RHIAddressMode addressV = RHIAddressMode::Wrap;
        RHIAddressMode addressW = RHIAddressMode::Wrap;

        // mip-map option
        f32 mipLODBias = 0.0f;
        f32 minLOD = 0.0f, maxLOD = std::numeric_limits<f32>::max();

        u32 maxAnisotropy = 1;
        RHIComparisonFunc compareFunc = RHIComparisonFunc::Never;
        Color borderColor = Colors::Black;
    };

    constexpr RHISamplerState LINEAR_WRAP_SAMPLER{
        .minFilter = RHIFilter::Linear,
        .magFilter = RHIFilter::Linear,
        .mipFilter = RHIFilter::Linear
    };
    constexpr RHISamplerState LINEAR_CLAMP_SAMPLER{
        .minFilter = RHIFilter::Linear,
        .magFilter = RHIFilter::Linear,
        .mipFilter = RHIFilter::Linear,
        .addressU = RHIAddressMode::Clamp,
        .addressV = RHIAddressMode::Clamp,
        .addressW = RHIAddressMode::Clamp
    };
    constexpr RHISamplerState NEAREST_WRAP_SAMPLER{
        .minFilter = RHIFilter::Nearest,
        .magFilter = RHIFilter::Nearest,
        .mipFilter = RHIFilter::Nearest
    };

    constexpr u32 RHI_FRAMES_IN_FLIGHT = 3;

    struct RHISwapchainCreateDesc{
        void* sdlWindow = nullptr;
        RHITextureCreateDesc bufferDesc;
        u32 bufferCount = RHI_FRAMES_IN_FLIGHT; // Triple buffering
        bool vsync = true;                           // VSync enabled by default
        bool allowTearing = false;                   // Variable refresh rate
    #if defined(_DEBUG) || !defined(NDEBUG)
        Str debugName;
    #endif
    };

    inline constexpr u32 getBytesPerPixel(RHIPixelFormat format){
        using enum RHIPixelFormat;

        switch(format){
        case R8_UNORM:          [[fallthrough]];
        case R8_SNORM:          [[fallthrough]];
        case R8_UINT:           [[fallthrough]];
        case R8_SINT:
            return 1;
        case R16_UNORM:         [[fallthrough]];
        case R16_SNORM:         [[fallthrough]];
        case R16_UINT:          [[fallthrough]];
        case R16_SINT:          [[fallthrough]];
        case R16_FLOAT:         [[fallthrough]];
        case RG8_UNORM:         [[fallthrough]];
        case RG8_SNORM:         [[fallthrough]];
        case RG8_UINT:          [[fallthrough]];
        case RG8_SINT:
            return 2;
        case R32_UINT:          [[fallthrough]];
        case R32_SINT:          [[fallthrough]];
        case R32_FLOAT:         [[fallthrough]];
        case RG16_UNORM:        [[fallthrough]];
        case RG16_SNORM:        [[fallthrough]];
        case RG16_UINT:         [[fallthrough]];
        case RG16_SINT:         [[fallthrough]];
        case RG16_FLOAT:        [[fallthrough]];
        case RGBA8_UNORM:       [[fallthrough]];
        case RGBA8_UNORM_SRGB:  [[fallthrough]];
        case RGBA8_SNORM:       [[fallthrough]];
        case RGBA8_UINT:        [[fallthrough]];
        case RGBA8_SINT:        [[fallthrough]];
        case BGRA8_UNORM:       [[fallthrough]];
        case BGRA8_UNORM_SRGB:
            return 4;
        case RG32_UINT:         [[fallthrough]];
        case RG32_SINT:         [[fallthrough]];
        case RG32_FLOAT:        [[fallthrough]];
        case RGBA16_UNORM:      [[fallthrough]];
        case RGBA16_SNORM:      [[fallthrough]];
        case RGBA16_UINT:       [[fallthrough]];
        case RGBA16_SINT:       [[fallthrough]];
        case RGBA16_FLOAT:
            return 8;
        case RGB32_FLOAT:
            return 12;
        case RGBA32_UINT:       [[fallthrough]];
        case RGBA32_SINT:       [[fallthrough]];
        case RGBA32_FLOAT:
            return 16;
        case D16_UNORM:
            return 2;
        case D24_UNORM_S8_UINT: [[fallthrough]];
        case D32_FLOAT:
            return 4;
        case D32_FLOAT_S8_UINT:
            return 8;
        default:
            std::unreachable();
        }
    }

    inline constexpr auto isDepthFormat(RHIPixelFormat format){
        using enum RHIPixelFormat;

        switch(format){
        case D16_UNORM:         [[fallthrough]];
        case D24_UNORM_S8_UINT: [[fallthrough]];
        case D32_FLOAT:         [[fallthrough]];
        case D32_FLOAT_S8_UINT:
            return true;
        default:
            return false;
        }
    }

    enum class RHIBindingAccess: u8{
        ReadOnly = 0,
        ReadWrite = 1,
        WriteOnly = 2
    };

    struct RHIBufferViewDesc{
        struct RawConfig{
            bool operator==(const RawConfig&) const = default;
        };
        struct TypedConfig{
            RHIPixelFormat format = RHIPixelFormat::Unknown;

            bool operator==(const TypedConfig&) const = default;
        };
        struct StructuredConfig{
            u32 stride = 0;

            bool operator==(const StructuredConfig&) const = default;
        };
        using Config = std::variant<RawConfig, TypedConfig, StructuredConfig>;

        u32 offset = 0, size = 0;
        Config config = RawConfig{};

        bool operator==(const RHIBufferViewDesc&) const = default;
    };

    struct RHITextureViewDesc{
        // TODO.
        RHIPixelFormat format = RHIPixelFormat::Unknown;

        bool operator==(const RHITextureViewDesc&) const = default;
    };

    struct RHISlotBindingInfo{
        u32 index = std::numeric_limits<u32>::max();
        RHIBindingAccess access = RHIBindingAccess::ReadOnly;
    };

    struct RHIShaderBindingInfo{
        StringHashMap<RHISlotBindingInfo> bufferInfo;
        StringHashMap<RHISlotBindingInfo> textureInfo;
        StringHashMap<RHISlotBindingInfo> samplerInfo;
    };

    struct RHIGraphicsBindingInfo{
        RHIShaderBindingInfo vsInfo;
        RHIShaderBindingInfo fsInfo;
    };

    struct RHIComputeBindingInfo{
        RHIShaderBindingInfo csInfo;
        RHISize3D reflectedSize = RHISize3D{1, 1, 1};
    };
}

template<>
struct std::hash<Smol::RHIBufferViewDesc>{
    std::size_t operator()(const Smol::RHIBufferViewDesc& desc) const noexcept{
        using namespace Smol;

        auto h = hashAll(
            std::hash<u32>{}(desc.offset),
            std::hash<u32>{}(desc.size),
            std::hash<usize>{}(desc.config.index())
        );

        using TypedConfig = RHIBufferViewDesc::TypedConfig;
        using StructuredConfig = RHIBufferViewDesc::StructuredConfig;

        std::visit([&h](const auto& cfg){
            using T = std::decay_t<decltype(cfg)>;
            if constexpr(std::is_same_v<T, TypedConfig>){
                hashCombine(h, std::hash<u32>{}(static_cast<u32>(cfg.format)));
            }
            else if constexpr(std::is_same_v<T, StructuredConfig>){
                hashCombine(h, std::hash<u32>{}(cfg.stride));
            }
        }, desc.config);
        return h;
    }
};

template<>
struct std::hash<Smol::RHITextureViewDesc>{
    std::size_t operator()(const Smol::RHITextureViewDesc& desc) const noexcept{
        using namespace Smol;

        return Smol::hashAll(
            static_cast<std::underlying_type_t<RHIPixelFormat>>(desc.format)
        );
    }
};
