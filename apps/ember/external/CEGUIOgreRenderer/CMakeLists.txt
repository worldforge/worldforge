add_library(ember-external-CEGUIOgreRenderer OBJECT
        CEGUIOgreRenderer/GeometryBuffer.cpp
        CEGUIOgreRenderer/ImageCodec.cpp
        CEGUIOgreRenderer/Renderer.cpp
        CEGUIOgreRenderer/ResourceProvider.cpp
        CEGUIOgreRenderer/Texture.cpp
        CEGUIOgreRenderer/TextureTarget.cpp
        CEGUIOgreRenderer/WindowTarget.cpp
)

target_include_directories(ember-external-CEGUIOgreRenderer PUBLIC .)
