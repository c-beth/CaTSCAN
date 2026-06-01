
add_subdirectory(hin)


add_executable(CaTSCAN
        CaTSCAN.cpp
)

target_link_libraries(CaTSCAN
        hin_lib
        GraphletInfo_lib
        ThreeNode_lib
        FourNode_lib
        FiveNode_lib
)


add_executable(Rossi
        Rossi.cpp
)

target_link_libraries(Rossi
        hin_lib
        GraphletInfo_lib
        Rossi_lib
)


add_executable(Shuffle
        Shuffle.cpp
)

target_link_libraries(Shuffle
        hin_lib
        GraphletInfo_lib
        ThreeNode_lib
        FourNode_lib
        FiveNode_lib
)