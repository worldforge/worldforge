from conan import ConanFile, tools
from conan.errors import ConanException
import os
import sys
from conan.tools.layout import basic_layout
from conan.tools.microsoft import MSBuild, MSBuildToolchain
from conan.tools.scm import Version
from conan.tools.gnu import Autotools, AutotoolsToolchain
from conan.tools.files import get, copy, collect_libs
from conan.tools.files import patch

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
    user = "worldforge"
    package_type = "library"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC
        if self.settings.compiler == "msvc":
            del self.options.shared

    def source(self):
        get(self, "https://download.gnome.org/sources/libsigc++/{}/{}.tar.xz".format(
            self.version.rpartition(".")[0],
            self.sourcename,
        ), strip_root=True)

        patch(self, patch_file="msvc.patch")

    def layout(self):
        basic_layout(self, src_folder=self.sourcename)

    def generate(self):
        if self.settings.compiler == "msvc":
            tc = MSBuildToolchain(self)
            tc.generate()
        else:
            tc = AutotoolsToolchain(self)
            tc.generate()

    def build(self):
        if self.settings.compiler == "msvc":
            msbuild = MSBuild(self)
            msbuild.build(os.path.join("MSVC_Net2013", "libsigc++2.sln"),
                          platforms=self.platforms,
                          toolset=self.settings.compiler.toolset)
        else:
            autotools = Autotools(self)
            args = (['--enable-shared', '--disable-static']
                    if self.options.shared else
                    ['--enable-static', '--disable-shared'])
            autotools.configure(args=args)
            autotools.make()
            autotools.install()

    def package(self):
        if self.settings.compiler == "msvc":
            copy(self, "*", src=os.path.join("vs12", self.platforms[str(self.settings.arch)]), dst=self.package_folder)
            if self.settings.build_type == "Debug":
                os.rename(os.path.join(self.package_folder, 'lib/sigc-vc120-d-2_0.lib'), os.path.join(self.package_folder, 'lib/sigc-2.0.lib'))
            else:
                os.rename(os.path.join(self.package_folder, 'lib/sigc-vc120-2_0.lib'), os.path.join(self.package_folder, 'lib/sigc-2.0.lib'))
            #os.rename(os.path.join(self.package_folder, 'bin/sigc-vc120-2_0.dll'), os.path.join(self.package_folder, 'bin/sigc-2.0.dll'))
            #os.rename(os.path.join(self.package_folder, 'bin/sigc-vc120-2_0.pdb'), os.path.join(self.package_folder, 'bin/sigc-2.0.pdb'))

        copy(self, "COPYING", dst="license", src=self.sourcename, keep_path=False)

    def package_info(self):
        self.cpp_info.includedirs = [os.path.join("include", "sigc++-2.0"),
                                     os.path.join("lib", "sigc++-2.0", "include")]
        self.cpp_info.libs = collect_libs(self)
