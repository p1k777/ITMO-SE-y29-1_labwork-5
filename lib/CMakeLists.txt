set(CMAKE_CXX_STANDARD 20)

add_library(itmoloops_lib
    fileparser.cpp
    defines.cpp

    instruments/instrument.cpp
    instruments/oscillator.cpp
    instruments/sampler.cpp
    instruments/sine.cpp
    instruments/square.cpp
    instruments/triangle.cpp

    effects/effect.cpp
    effects/echo.cpp
    effects/gain.cpp
    effects/tremolo.cpp

    wavwriter.cpp
)
target_include_directories(itmoloops_lib PUBLIC .)
