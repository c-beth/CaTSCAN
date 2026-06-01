
add_library(ThreeNode_lib
        ThreeNode.hpp
        ThreeNode.cpp
)

target_link_libraries(ThreeNode_lib
        hin_lib
        GraphletInfo_lib
)
