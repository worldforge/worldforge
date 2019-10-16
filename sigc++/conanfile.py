from conans import ConanFile, tools, MSBuild, AutoToolsBuildEnvironment
from conans.tools import Version
from conans.errors import ConanException
import os.path
import sys


class SigcppConan(ConanFile):
    name = "sigc++"
    version = "2.10.0"
    license = "LGPL-3.0"
    url = "https://github.com/bincrafters/conan-sigcpp"
    homepage = "https://github.com/libsigcplusplus/libsigcplusplus"
    description = "Typesafe Callback Framework for C++"
    author = "Bincrafters <bincrafters@gmail.com>"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {'shared': False, 'fPIC': True}
    exports = "LICENSE.md"
    exports_sources = "msvc.patch"

    sourcename = "libsigc++-{}".format(version)
    platforms = {"x86": "Win32",
                 "x86_64": "x64"}

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        if self.settings.compiler == "Visual Studio":
            del self.options.shared

    @property
    def is_python2(self):
        return sys.version_info[0] == 2

    @property
    def supports_cpp14(self):
        compiler = str(self.settings.compiler)
        version = Version(str(self.settings.compiler.version))
        if compiler == "Visual Studio" and version >= Version("14"):
            return True
        if compiler == "gcc" and version >= Version("5"):
            return True
        if compiler == "clang" and version >= Version("3.4"):
            return True
        if compiler == "apple-clang" and version >= Version("6.1"):
            return True
        return False

    def configure(self):
        if not self.supports_cpp14:
            raise ConanException("The specified compiler must support C++14")

    def build_requirements(self):
        if self.settings.os == "Windows" and self.is_python2:
            self.build_requires("7z_installer/1.0@conan/stable")

    def source(self):
        tools.download("https://download.gnome.org/sources/libsigc++/{}/{}.tar.xz".format(
            self.version.rpartition(".")[0],
            self.sourcename,
        ), self.sourcename + ".tar.xz")

        if self.is_python2:
            if self.settings.os == "Windows":
                self.run("7z x {}.tar.xz".format(self.sourcename))
                os.remove(self.sourcename + ".tar.xz")
            else:
                self.run("xz -d {}.tar.xz".format(self.sourcename))
            tools.unzip(self.sourcename + ".tar")
            os.remove(self.sourcename + ".tar")
        else:
            tools.unzip(self.sourcename + ".tar.xz")
            os.remove(self.sourcename + ".tar.xz")

        tools.patch(base_path=self.sourcename, patch_file="msvc.patch")

    def build(self):
        with tools.chdir(self.sourcename):
            if self.settings.compiler == "Visual Studio":
                msbuild = MSBuild(self)
                msbuild.build(os.path.join("MSVC_Net2013", "libsigc++2.sln"),
                              platforms=self.platforms,
                              toolset=self.settings.compiler.toolset)
            else:
                autotools = AutoToolsBuildEnvironment(self)
                args = (['--enable-shared', '--disable-static']
                        if self.options.shared else
                        ['--enable-static', '--disable-shared'])
                autotools.configure(args=args)
                autotools.make()
                autotools.install()

    def package(self):
        if self.settings.compiler == "Visual Studio":
            self.copy("*", src=os.path.join("vs12", self.platforms[str(self.settings.arch)]))
        self.copy("COPYING", dst="license", src=self.sourcename, keep_path=False)

    def package_info(self):
        self.cpp_info.includedirs = [os.path.join("include", "sigc++-2.0"),
                                     os.path.join("lib", "sigc++-2.0", "include")]
        self.cpp_info.libs = tools.collect_libs(self)
