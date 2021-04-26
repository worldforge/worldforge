#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, tools, CMake
import os
import glob


class LibgcryptConan(ConanFile):
    name = "libgcrypt"
    version = "1.8.4"
    url = "http://github.com/bincrafters/conan-libgcrypt"
    author = "Bincrafters <bincrafters@gmail.com>"
    description = "Libgcrypt is a general purpose cryptographic library originally based on code from GnuPG"
    license = "LGPL-2.1-or-later"
    homepage = "https://www.gnupg.org/download/index.html#libgcrypt"
    topics = ("conan", "libgcrypt", "gcrypt", "gnupg", "gpg", "crypto", "cryptography")
    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False],
               "fPIC": [True, False]}

    default_options = {"shared": False, "fPIC": True}
    _source_subfolder = "sources"
    _build_subfolder = "build"
    exports = ["LICENSE.md"]
    exports_sources = ["CMakeLists.txt", "patches/*.patch", "config.h.in", "gcrypt.h.in", "mod-source-info.h"]
    generators = "cmake"

    requires = 'libgpg-error/1.42@worldforge/stable'

    @property
    def _is_msvc(self):
        return self.settings.compiler == "Visual Studio"

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        del self.settings.compiler.libcxx

    def source(self):
        source_url = "https://gnupg.org/ftp/gcrypt/libgcrypt"
        tools.get("{0}/libgcrypt-{1}.tar.gz".format(source_url, self.version),
                  sha256="fc3c49cc8611068e6008482c3bbee6c66b9287808bbb4e14a473f4cc347b78ce")
        extracted_dir = self.name + "-" + self.version
        os.rename(extracted_dir, self._source_subfolder)

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure(build_dir=self._build_subfolder)
        return cmake

    def build(self):
        for filename in sorted(glob.glob("patches/*.patch")):
            self.output.info('applying patch "%s"' % filename)
            tools.patch(base_path=self._source_subfolder, patch_file=filename)
        if self._is_msvc:
            os.makedirs(os.path.join(self._source_subfolder, "sys"))
            tools.download("https://raw.githubusercontent.com/win32ports/unistd_h/master/unistd.h",
                           os.path.join(self._source_subfolder, "unistd.h"))
            tools.download("https://raw.githubusercontent.com/win32ports/sys_time_h/master/sys/time.h",
                           os.path.join(self._source_subfolder, "sys", "time.h"))
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        self.copy(pattern="COPYING*", src="sources", dst="licenses")
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["gcrypt"]
