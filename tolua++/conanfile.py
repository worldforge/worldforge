from conans import ConanFile, tools, CMake


class Lua(ConanFile):
    name = 'tolua++'
    version = '1.0.93'
    license = 'MIT'
    url = ''
    homepage = ''
    description = ''
    settings = 'os', 'arch', 'build_type', 'compiler'
    generators = 'cmake_paths'
    exports_sources = 'CMakeLists.txt'
    options = {'shared': [True, False]}
    default_options = {"shared": False}
    requires = (
        "lua/5.1.5@worldforge/testing"
    )

    def source(self):
        git = tools.Git()
        git.clone("https://github.com/worldforge/toluapp.git", "lua51")

    def build(self):
        cmake = CMake(self)
        cmake.definitions['CMAKE_TOOLCHAIN_FILE'] = 'conan_paths.cmake'
        cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = 'ON'
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("tolua++.h", dst="include", src="include")
        self.copy("tolua++", dst="bin", src="bin")
        self.copy('*.lib', dst='lib', keep_path=False)
        self.copy('*.dll', dst='lib', keep_path=False)
        self.copy('*.so', dst='lib', keep_path=False)
        self.copy('*.dylib', dst='lib', keep_path=False)
        self.copy('*.a', dst='lib', keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ['tolua++']

    # def package_info(self):
    #     # Lua modules should not link against the lua core, except on windows.
    #     # See http://lua-users.org/wiki/BuildingModules.
    #     shouldBeLinked = self.settings.os == "Windows" or not self.options.forModule
    #     if shouldBeLinked:
    #         self.cpp_info.libs = ['lua']
    #     self.cpp_info.libdirs = ['lib/lua5.1']
    #     self.cpp_info.includedirs = ["include/lua5.1"]
