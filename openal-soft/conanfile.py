from conans import ConanFile, CMake
from conans.tools import get, replace_in_file


class openal(ConanFile):
    name = "openal-soft"
    version = "1.19.1"
    FOLDER_NAME = "openal-soft-openal-soft-1.19.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    url = ""
    author = "Bartlomiej Parowicz (bparowicz@gmail.com)"
    exports = ["FindOpenAl.cmake"]
    license = "MIT License"

    # def system_requirements(self):
    #     if self.settings.os == "Linux":
    #         installer = SystemPackageTool()
    #         installer.install("libasound2-dev")

    def source(self):
        get("https://github.com/kcat/openal-soft/archive/openal-soft-1.19.1.tar.gz")
        replace_in_file("{0}/CMakeLists.txt".format(self.FOLDER_NAME), "PROJECT(OpenAL)", """PROJECT(OpenAL)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
""")

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=self.FOLDER_NAME)
        cmake.build()
        cmake.install()

    def package_info(self):
        if self.settings.os == "Windows":
            self.cpp_info.libs = ["OpenAL32"]
        else:
            self.cpp_info.libs = ["openal"]
