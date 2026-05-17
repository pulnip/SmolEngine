add_subdirectory(Example)

add_library(MyGame INTERFACE)

target_link_libraries(MyGame
INTERFACE
    "$<LINK_LIBRARY:WHOLE_ARCHIVE,ExampleLib>"
)

target_link_libraries(SmolApp
PRIVATE
    MyGame
)
