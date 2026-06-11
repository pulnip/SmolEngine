smol_declare_module(D2DCanvas)

target_precompile_headers(SmolD2DCanvas
PRIVATE
    <d2d1_3.h>
    <dwrite.h>
    <dxgi.h>
    <d3d11.h>
)

target_link_libraries(SmolD2DCanvas
PUBLIC
    Smol::Canvas2D
PRIVATE
    d2d1.lib
    dwrite.lib
    Smol::Log
)
