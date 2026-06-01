
add_library(FourNode_lib
        FourNode.hpp
        FourNodeCalculator.cpp
        ThreeStars.cpp
        ThreePaths.cpp
        TailedTriangles.cpp
        FourCycles.cpp
        Diamonds.cpp
        FourCliques.cpp
)

target_link_libraries(FourNode_lib
        hin_lib
        GraphletInfo_lib
        ThreeNode_lib
)
