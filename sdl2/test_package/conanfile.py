#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, CMake, tools, RunEnvironment
import os


class TestPackageConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"

    def build(self):
        self.build_cmake()

    def build_cmake(self):
        cmake = CMake(self)
        if self.settings.os == "Linux":
            cmake.definitions['WITH_X11'] = self.options['sdl2'].x11
            cmake.definitions['WITH_ALSA'] = self.options['sdl2'].alsa
            cmake.definitions['WITH_PULSE'] = self.options['sdl2'].pulse
            cmake.definitions['WITH_ESD'] = self.options['sdl2'].esd
            cmake.definitions['WITH_ARTS'] = self.options['sdl2'].arts
            cmake.definitions['WITH_DIRECTFB'] = self.options['sdl2'].directfb
        if self.settings.os == "Windows":
            cmake.definitions['WITH_DIRECTX'] = self.options['sdl2'].directx
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
