from conans import ConanFile, AutoToolsBuildEnvironment, tools
import os
import shutil


class GPGErrorConan(ConanFile):
    name = "libgpg-error"
    version = "1.42"
    homepage = "https://gnupg.org/software/libgpg-error/index.html"
    url = "http://github.com/bincrafters/conan-libgpg-error"
    topics = ("conan", "gpg", "gnupg")
    description = "Libgpg-error is a small library that originally defined common error values for all GnuPG " \
                  "components."
    license = "GPL-2.0-or-later"
    exports_sources = ["CMakeLists.txt"]
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    _source_subfolder = "sources"


    def configure(self):
        del self.settings.compiler.libcxx
        del self.settings.compiler.cppstd

    def source(self):
        source_url = "https://www.gnupg.org/ftp/gcrypt/libgpg-error"
        tools.get("{0}/libgpg-error-{1}.tar.bz2".format(source_url, self.version),
                  sha256="fc07e70f6c615f8c4f590a8e37a9b8dd2e2ca1e9408f8e60459c67452b925e23")
        extracted_dir = self.name + "-" + self.version
        os.rename(extracted_dir, self._source_subfolder)

    def build(self):

        # the previous step might hang when converting from ISO-8859-2 to UTF-8 late in the build process
        os.unlink(os.path.join(self._source_subfolder, "po", "ro.po"))
        build = None
        host = None
        rc = None
        env = dict()
        args = ["--disable-dependency-tracking",
                "--disable-nls",
                "--disable-languages",
                "--disable-doc",
                "--disable-tests"]
        if self.settings.os != "Windows" and self.options.fPIC:
            args.append("--with-pic")
        if self.options.shared:
            args.extend(["--disable-static", "--enable-shared"])
        else:
            args.extend(["--disable-shared", "--enable-static"])
        if self.settings.os == "Linux" and self.settings.arch == "x86":
            host = "i686-linux-gnu"

        with tools.chdir(self._source_subfolder):
            with tools.environment_append(env):
                env_build = AutoToolsBuildEnvironment(self, win_bash=tools.os_info.is_windows)
                env_build.configure(args=args, build=build, host=host)
                env_build.make()
                env_build.install()

    def package(self):
        self.copy(pattern="COPYING", dst="licenses", src=self._source_subfolder)
        la = os.path.join(self.package_folder, "lib", "libgpg-error.la")
        if os.path.isfile(la):
            os.unlink(la)

    def package_info(self):
        self.cpp_info.libs = ["gpg-error"]
        self.env_info.PATH.append(os.path.join(self.package_folder, "bin"))
