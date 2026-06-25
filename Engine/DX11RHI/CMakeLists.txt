smol_declare_module(DX11RHI)

target_precompile_headers(SmolDX11RHI
PRIVATE
    <d3d11.h>
    <d3dcompiler.h>
    <dxgi1_6.h>
    <wrl/client.h>
)

target_link_libraries(SmolDX11RHI
PUBLIC
    Smol::CommonRHI
    Smol::RHI
PRIVATE
    d3d11.lib
    dxgi.lib
    d3dcompiler.lib
    dxguid.lib
    SDL3::SDL3
    Smol::Log
)
