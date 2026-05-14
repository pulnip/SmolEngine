add_executable(SmolMain)

target_sources(SmolMain
PRIVATE
    main.cpp
)

target_link_libraries(SmolMain
PRIVATE
    Smol::Core
    Smol::Reflection
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,ExampleActor>"
)
