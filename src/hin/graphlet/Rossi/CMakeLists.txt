
add_library(Rossi_lib
        ThreeNode.hpp
        ThreeNode.cpp
        FourNode.hpp
        FourNode.cpp
)

target_link_libraries(Rossi_lib
        hin_lib
        GraphletInfo_lib
)
