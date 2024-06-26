#[[
    MIT License

    Copyright (c) 2024 Alberto Morcillo Sanz

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
]]

project(RaytracingGL)

# Header Files
set(HEADERS 
    ptr.h
    vendor/json.hpp
    vendor/stb_image_write.h
    vendor/stb_image.h
    vendor/tiny_gltf.h
    geometry/vertex.h
    opengl/buffer/buffer.h
    opengl/shader/shader.h
)

# CPP files
set(SOURCES
    vendor/tiny_gltf.cc
    opengl/buffer/buffer.cpp
    opengl/shader/shader.cpp
    renderer/renderer.cpp
)

# Compile files
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
                assimp 
                $<$<BOOL:${UNIX}>:GL>
                $<$<BOOL:${UNIX}>:dl>
                $<$<BOOL:${UNIX}>:X11>
                GLEW::GLEW
)