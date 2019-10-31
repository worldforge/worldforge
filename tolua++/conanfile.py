from conans import ConanFile, tools, CMake


class ToLua(ConanFile):
    name = 'tolua++'
    version = '1.0.93'
    license = 'MIT'
    url = ''
    homepage = ''
    description = ''
    settings = 'os', 'arch', 'build_type', 'compiler'
    generators = 'cmake'
    exports_sources = 'CMakeLists.txt'
    options = {'shared': [True, False]}
    default_options = {"shared": False}
    requires = (
        "lua/5.1.5@worldforge/stable"
    )

    def source(self):
        git = tools.Git()
        git.clone("https://github.com/worldforge/toluapp.git", "lua51")

    def build(self):
        tools.replace_in_file("CMakeLists.txt", "project(tolua++)", """project(tolua++)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
""")
        cmake = CMake(self)
        cmake.definitions['CMAKE_POSITION_INDEPENDENT_CODE'] = 'ON'
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("tolua++.h", dst="include", src="include")
        self.copy("*", dst="bin", src="bin")
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
