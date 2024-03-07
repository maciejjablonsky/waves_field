from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
import os

class waves_field(ConanFile):
    name = "waves_field"
    version = "0.1"
    package_type = "application"

    license = "MIT"
    author = "Maciek Jablonski maciekjablonsky@gmail.com"
    topics = ("opengl", "sea", "simulation")

    settings = "os", "compiler", "build_type", "arch"

    exports_sources = "CMakeLists.txt", "src/*"

    default_options = {
        'boost/*:header_only': True
    }

    def layout(self):
        cmake_layout(self)

    def build_requirements(self):
        self.tool_requires("ninja/1.11.1")

    def requirements(self):
        self.requires("fmt/10.1.1")
        self.requires("glfw/3.3.8")
        self.requires("glew/2.2.0")
        self.requires("glm/cci.20230113")
        self.requires("rapidjson/cci.20230929")
        self.requires("entt/3.12.2")
        self.requires("tinyobjloader/2.0.0-rc10")
        self.requires("ms-gsl/4.0.0")
        # self.requires("mp-units/2.1.0")
        self.requires("magic_enum/0.9.5")
        self.requires("freetype/2.13.2")
        self.requires("boost/1.84.0")
        self.requires("range-v3/0.12.0")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

        self._update_cmake_presets()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def _update_cmake_presets(self):
        cmake_presets_path = os.path.join(
            self.build_folder, "generators", "CMakePresets.json"
        )
        assert os.path.exists(cmake_presets_path)

        self._add_cmake_executable_to_cmake_presets(cmake_presets_path)
        self._add_ninja_executable_to_cmake_presets(cmake_presets_path)

    def _add_cmake_executable_to_cmake_presets(self, cmake_presets_path):
        import shutil
        cmake_presets = self._load_json_from_file(cmake_presets_path)

        cmake_path = os.path.dirname(shutil.which("cmake"))
        self.output.info(
            f'Setting "cmakeExecutable" in {cmake_presets_path} to "{cmake_path}"'
        )
        for configuration_preset in cmake_presets["configurePresets"]:
            configuration_preset["cmakeExecutable"] = os.path.join(
                cmake_path, "cmake.exe"
            )
        self._save_json_to_file(cmake_presets_path, cmake_presets)

    def _add_ninja_executable_to_cmake_presets(self, cmake_presets_path):
        cmake_presets = self._load_json_from_file(cmake_presets_path)
        ninja_path = self.dependencies.build["ninja"].cpp_info.bindirs[0]
        system_path = os.environ.get("PATH", "")  # Get the current system PATH

        new_path = ninja_path
        if system_path:
            new_path += os.pathsep + system_path
        for configuration_preset in cmake_presets["configurePresets"]:
            if "environment" not in configuration_preset:
                configuration_preset["environment"] = {}
            if "PATH" in configuration_preset["environment"]:
                configuration_preset["environment"]["PATH"] += os.pathsep + new_path
            else:
                configuration_preset["environment"]["PATH"] = new_path
        self._save_json_to_file(cmake_presets_path, cmake_presets)

    def _load_json_from_file(self, path):
        import json

        with open(path, "r") as file:
            return json.load(file)

    def _save_json_to_file(self, path, content):
        import json

        with open(path, "w") as file:
            file.write(json.dumps(content, indent=4))
