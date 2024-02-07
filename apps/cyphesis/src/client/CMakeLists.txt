add_subdirectory(cyclient)
add_subdirectory(aiclient)


add_library(cyphesis-client
        ClientPropertyManager.cpp
        BaseClient.cpp
        SimpleTypeStore.cpp
)

target_link_libraries(cyphesis-client PUBLIC
        cyphesis-common)
