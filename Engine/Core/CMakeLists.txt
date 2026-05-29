smol_declare_module(Core)

if(SMOL_ENABLE_TEST)
    smol_declare_test(Core
        DIRECTORY Test
        DEPENDS Smol::Core
        LABELS "unit"
    )
endif()
