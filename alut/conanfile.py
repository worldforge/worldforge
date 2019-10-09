import fnmatch
import os
import shutil

from conans import ConanFile, CMake, tools


def apply_patches(source, dest):
    for root, dirnames, filenames in os.walk(source):
        for filename in fnmatch.filter(filenames, '*.patch'):
            patch_file = os.path.join(root, filename)
            dest_path = os.path.join(dest, os.path.relpath(root, source))
            tools.patch(base_path=dest_path, patch_file=patch_file)


class AlutConan(ConanFile):
    name = "alut"
    description = "Freealut library for OpenAL"
    version = "1.1.0"
    folder = 'freealut-fc814e316c2bfa6e05b723b8cc9cb276da141aae'
    generators = "cmake"
    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False]
    }
    default_options = {"shared": False}
    exports = ["CMakeLists.txt", "patches*"]
    requires = (
        "openal/1.19.0@bincrafters/stable"
    )
    url = ""
    license = "https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html"

    def source(self):
        tools.get("https://github.com/vancegroup/freealut/archive/fc814e316c2bfa6e05b723b8cc9cb276da141aae.zip")
        apply_patches('patches', self.folder)

    def build(self):
        syslibs = ''
        if self.settings.os == 'Windows':
            syslibs = 'Winmm'
        elif self.settings.os == 'Linux':
            syslibs = 'pthread dl'
        alut_cmakelists = "{0}/src/CMakeLists.txt".format(self.folder)
        tools.replace_in_file("{0}/CMakeLists.txt".format(self.folder), "project(Alut C)", """project(Alut C)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
""")
        tools.replace_in_file(alut_cmakelists,
                              'target_link_libraries(alut ${OPENAL_LIBRARY})',
                              'target_link_libraries(alut ${OPENAL_LIBRARY} %s)' % syslibs)
        # There are some issues with copying files on macos...
        tools.replace_in_file(alut_cmakelists, 'if(NOT WIN32)', 'if(FALSE)')

        cmake = CMake(self)
        cmake.definitions['BUILD_EXAMPLES'] = False
        cmake.definitions['BUILD_STATIC'] = not self.options.shared
        cmake.definitions['BUILD_TESTS'] = False
        cmake.configure(source_dir=self.folder)
        cmake.build()
        cmake.install()

    def package(self):
        # If we're building a static library we need to copy it ourselves, and remove the shared one.
        if not self.options.shared:
            os.remove("{}/lib/libalut.so".format(self.package_folder))
            os.remove("{}/lib/libalut.so.0".format(self.package_folder))
            shutil.copy("lib/libalut_static.a", "{}/lib/libalut.a".format(self.package_folder))

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        if self.settings.os == 'Linux':
            self.cpp_info.libs.extend(['dl', 'pthread', 'm'])
