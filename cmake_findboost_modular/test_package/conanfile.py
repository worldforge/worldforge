#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, CMake, tools, RunEnvironment
import os


class TestPackageConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    requires = ("boost_regex/1.69.0@bincrafters/testing",
                "boost_system/1.69.0@bincrafters/testing",
                "boost_date_time/1.69.0@bincrafters/testing",  # part of group
                "boost_asio/1.69.0@bincrafters/testing")  # header-only


    def build(self):
        for build_variant in ['FIND_PACKAGE', 'FIND_PACKAGE_TARGETS']:
            for conan_variant in ['CONAN', 'CONAN_TARGETS']:
                cmake = CMake(self)
                cmake.definitions['CONAN_VARIANT'] = conan_variant
                cmake.definitions['BUILD_VARIANT'] = build_variant
                cmake.configure()
                cmake.build()

    def test(self):
        with tools.environment_append(RunEnvironment(self).vars):
            bin_path = os.path.join("bin", "test_package")
            if self.settings.os == "Windows":
                self.run(bin_path)
            elif self.settings.os == "Macos":
                self.run("DYLD_LIBRARY_PATH=%s %s" % (os.environ.get('DYLD_LIBRARY_PATH', ''), bin_path))
            else:
                self.run("LD_LIBRARY_PATH=%s %s" % (os.environ.get('LD_LIBRARY_PATH', ''), bin_path))
