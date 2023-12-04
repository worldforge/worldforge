import os

from conan import ConanFile
from conan.tools.cmake import cmake_layout, CMakeDeps, CMakeToolchain
from conan.tools.microsoft import is_msvc


class Worldforge(ConanFile):
    version = "0.1.0"
    package_type = "application"
    settings = "os", "arch", "compiler", "build_type"
    url = "https://github.com/worldforge"
    homepage = "https://www.worldforge.org"
    name = "Worldforge"
    license = "GPL-3.0-or-later"
    author = "Erik Ogenvik <erik@ogenvik.org>"

    options = {
        "with_client": [True, False],
        "with_server": [True, False],
        "without_metaserver_server": [True, False]
    }

    default_options = {
        "with_client": True,
        "with_server": True,
        #Normally you don't want to build the actual metaserver server.
        "without_metaserver_server": True,
        # Skipped because we had issues building xz_utils, which is used by libunwind
        'sdl/*:libunwind': False,
        # We got build errors with X11 on OpenSuSE, so disable for now. Unclear what the X11 support entails.
        'pulseaudio/*:with_x11': False,
        # Unclear why the pulseaudio client lib needs openssl...
        'pulseaudio/*:with_openssl': False,
        # We're getting compilation errors on Ubuntu 22.04 with "mpg123" so we'll disable that. Might want to enable
        # again when we enable better sound support.
        'libsndfile/*:with_mpeg': False
    }

    def config_options(self):
        if is_msvc(self):
            self.options.with_server = False

    def requirements(self):
        self.requires("bullet3/2.89")
        self.requires("libsigcpp/3.0.7")
        self.requires("libcurl/8.4.0")
        self.requires("spdlog/1.12.0")
        self.requires("cli11/2.3.2")
        self.requires("boost/1.83.0")

        self.requires("zlib/1.3")
        self.requires("bzip2/1.0.8")

        if self.options.with_client:
            self.requires("cegui/0.8.7@worldforge")
            self.requires("ogre/13.4.2@worldforge")
            self.requires("sdl/2.28.5")
            self.requires("lua/5.3.6")
            self.requires("libunwind/1.7.2")


        if self.options.with_server:
            self.requires("worldforge-worlds/0.1.0@worldforge")
            self.requires("libgcrypt/1.8.4")
            self.requires("sqlite3/3.44.2")
            self.requires("readline/8.1.2")
            self.requires("cpython/3.10.0@worldforge")
            # self.requires("avahi/0.8")

        if not is_msvc(self):
            self.requires("libxdg-basedir/1.2.3@worldforge")

        self.requires("libcap/2.69", override=True)
        self.requires("libxml2/2.11.5", override=True)
        self.requires("libxcrypt/4.4.36", override=True)

        self.test_requires("cppunit/1.15.1")
        self.test_requires("catch2/3.4.0")

    def generate(self):
        deps = CMakeDeps(self)
        # OGRE provides its own CMake files which we should use
        deps.set_property("ogre", "cmake_find_mode", "none")
        deps.generate()

        tc = CMakeToolchain(self)
        # We need to do some stuff differently if Conan is in use, so we'll tell the CMake system this.
        tc.variables["CONAN_FOUND"] = "TRUE"
        if not self.options.with_client:
            tc.variables["SKIP_EMBER"] = "TRUE"
        if not self.options.with_server:
            tc.variables["SKIP_CYPHESIS"] = "TRUE"
        else:
            tc.variables["PYTHON_IS_STATIC"] = "TRUE"
            # The default CMake FindPython3 component will set the Python3_EXECUTABLE, which is then used in Cyphesis.
            # Therefore, do this here.
            tc.variables["Python3_EXECUTABLE"] = os.path.join(self.dependencies["cpython"].package_folder, "bin/python")
            # Inject the PYTHOHOME variable so we can copy the Python scripts to the Snap package if we choose to build
            # that.
            tc.variables["PYTHONHOME"] = self.dependencies["cpython"].package_folder
            tc.variables["WORLDFORGE_WORLDS_SOURCE_PATH"] = os.path.join(
                self.dependencies["worldforge-worlds"].package_folder, "worlds")
            tc.preprocessor_definitions["PYTHONHOME"] = "\"{}\"".format(self.dependencies["cpython"].package_folder)

        if not self.options.without_metaserver_server:
            tc.variables["BUILD_METASERVER_SERVER"] = "TRUE"

        tc.generate()

    def layout(self):
        cmake_layout(self)
