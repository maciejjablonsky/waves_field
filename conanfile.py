from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class waves_field(ConanFile):
    name = "waves_field"
    version = "0.1"
    package_type = "application"

    license = "MIT"
    author = "Maciek Jablonski maciekjablonsky@gmail.com"
    topics = ("opengl", "sea", "simulation")

    settings = "os", "compiler", "build_type", "arch"

    exports_sources = "CMakeLists.txt", "src/*"

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        self.requires("fmt/10.1.1")
        self.requires("glfw/3.3.8")
        self.requires("glad/0.1.36")
        self.requires("glm/cci.20230113")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
