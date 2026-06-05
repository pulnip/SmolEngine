smol_declare_module(Reflection)

target_link_libraries(SmolReflection
PUBLIC
    Smol::Core
    Smol::Serialization
)

if(SMOL_ENABLE_TEST)
    smol_declare_test(Reflection
        DIRECTORY Test
        DEPENDS "$<LINK_LIBRARY:WHOLE_ARCHIVE,Smol::Reflection>"
        LABELS "unit"
    )
endif()
