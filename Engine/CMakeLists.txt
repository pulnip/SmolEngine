# 1) Core Layer
add_subdirectory(Core)

# 2-0) Infrastructure Layer
add_subdirectory(Log)
add_subdirectory(Platform)
add_subdirectory(Reflection)
add_subdirectory(RHI)
add_subdirectory(Serialization)
# 2-1) Infrastructure Implement Layer
if(RENDER_BACKEND STREQUAL "DX11")
    add_subdirectory(DX11RHI)
elseif(RENDER_BACKEND STREQUAL "Metal")
    add_subdirectory(MetalRHI)
endif()

# 3-0) Subsystem Layer
add_subdirectory(Input)
add_subdirectory(Render)
# add_subdirectory(Scene)
# 3-1) Gameplay Layer
add_subdirectory(Gameplay)
