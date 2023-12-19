add_library(ember-assets
        AssetsUpdater.cpp
)

target_link_libraries(ember-assets PUBLIC
        squallcore
        squallcurl
        libsigcpp::sigc++
        Boost::headers
)

