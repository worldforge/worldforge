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
        # Normally you don't want to build the actual metaserver server.
        "without_metaserver_server": True,
        # Unclear why the pulseaudio client lib needs openssl...
        'pulseaudio/*:with_openssl': False,
        # We're not using any mpeg stuff so no need to include that
        'libsndfile/*:with_mpeg': False,
        # Build Python without any extra stuff.
        'cpython/*:with_bz2': False,
        'cpython/*:with_gdbm': False,
        'cpython/*:with_sqlite3': False,
        'cpython/*:with_tkinter': False,
        'cpython/*:with_lzma': False,
    }

    def config_options(self):
        if is_msvc(self):
            self.options.with_server = False

    def requirements(self):

        self.requires("libsigcpp/3.0.7")
        self.requires("libcurl/8.9.1")
        self.requires("spdlog/1.14.1")
        self.requires("cli11/2.4.2")
        self.requires("boost/1.85.0")

        self.requires("zlib/1.3.1")
        self.requires("bzip2/1.0.8")

        if self.options.with_client or self.options.with_server:
            self.requires("bullet3/2.89")

        if self.options.with_client:
            self.requires("cegui/0.8.7@worldforge")
            self.requires("ogre/14.2.6@worldforge")
            self.requires("sdl/2.30.5")
            self.requires("lua/5.3.6")
            self.requires("vorbis/1.3.7")

            if not is_msvc(self):
                self.requires("libunwind/1.8.1", force=True)

        if self.options.with_server:
            self.requires("worldforge-worlds/0.1.0@worldforge")
            self.requires("libgcrypt/1.10.3")
            self.requires("sqlite3/3.46.0", force=True)
            self.requires("readline/8.2")
            self.requires("cpython/3.12.2")

        # self.requires("avahi/0.8")

        if not is_msvc(self):
            self.requires("libxdg-basedir/1.2.3@worldforge")

        self.requires("expat/2.6.2", override=True)

        self.test_requires("cppunit/1.15.1")
        self.test_requires("catch2/3.6.0")

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
