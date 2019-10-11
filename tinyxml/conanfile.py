from conans import ConanFile, tools, CMake
import os
from pathlib import Path

class TinyXML(ConanFile):
    name = 'tinyxml'
    version = '2.6.2'
    license = 'zlib'
    url = ''
    homepage = ''
    description = ''
    settings = 'os', 'arch', 'build_type', 'compiler'
    options = {'shared': [True, False]}
    default_options = {"shared": False}
    generators = ['cmake']

    source_subfolder = "source"
    commit_id = "9e0c171e76de9e0373377a7a8c869873c85d5c93"


    def source(self):
        tools.get("https://github.com/worldforge/tinyxml/archive/{0}.tar.gz".format(self.commit_id))
        os.rename("tinyxml-{0}".format(self.commit_id), self.source_subfolder)

    def build(self):
        
        tools.replace_in_file("{0}/CMakeLists.txt".format(self.source_subfolder), "project(tinyxml)", """project(tinyxml)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
""")
        cmake = CMake(self)

        cmake.definitions['BUILD_SHARED_LIBS'] = self.options.shared

        if not tools.os_info.is_windows:
            cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = 'ON'

        cmake.configure(source_folder=self.source_subfolder)
        cmake.build()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
