# Build Environment 

## Quick Commands

```bash
cmake -S . -B build # configure project
cmake --build build # build project
./build/bin/CHAPTERNAME # launch chapter
cmake --install build --prefix install # install project
```

## Editor/IDE

Choice is up to you. I am using VsCode and [CMake extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools). 
If [CMakePresets file](./CMakePresets.json) doesn't include your desired environment you can use CMake itself by terminal or gui without presets or modify the file.

## CMake

Download latest CMake from [here](https://cmake.org/download/) according to your OS.

**Note:** To work with Android you will need:

* [Android NDK](https://developer.android.com/ndk/downloads)
* Android SDK: Download [Android Studio](https://developer.android.com/studio) and use SDK Manager tool or use [*sdkmanager*](https://developer.android.com/studio#command-line-tools-only) cli tool
* Also set Environment Variable `ANDROID_NDK_HOME` to the installation location of the SDK.

**Note:** To work with Emscripten (Web Builds) you will need [Emscripten SDK](https://emscripten.org/docs/getting_started/downloads.html).

* Also set Environment Variable `EMSDK` to the installation location of the SDK.
  


## 1. Listing Available Presets

To see a list of all available presets, you can use the following command in the root directory of your project (where the `CMakePresets.json` file is located):

```bash
cmake --list-presets
```

To list presets of a specific type (e.g., `configure`, `build`, `test`, `package`, or `workflow`), you can use:

```bash
cmake --list-presets=<type>
```

For example, to list only the configure presets:

```bash
cmake --list-presets=configure
```

## 2. Configuring the Project

To configure your project with a specific preset, use the `cmake --preset` command. This will generate the build files in the directory specified by the `binaryDir` variable in the preset.

**Syntax:**

```bash
cmake --preset <preset-name>
```

**Examples:**

* **Windows (Debug):**
  
```bash
cmake --preset windows-x64-debug
```

* **Linux (Release):**
  
```bash
cmake --preset linux-x64-release
```

* **macOS (Apple Silicon, Debug):**

```bash
cmake --preset macos-arm64-debug
```

* **Android (ARM64 Debug for Chapter 1):**

```bash
cmake --preset android-debug-chapter01
```

* **Web (Debug):**

```bash
cmake --preset web-debug
```

### 3. Building the Project

After configuring the project, you can build it using a build preset. Build presets are associated with a configure preset.

**Syntax:**

```bash
cmake --build --preset <build-preset-name>
```

**Examples:**

* **Windows (Debug):**

```bash
cmake --build --preset windows-x64-debug
```

* **Linux (Release):**

```bash
cmake --build --preset linux-x64-release
```

* **macOS (Apple Silicon, Debug):**

```bash
cmake --build --preset macos-arm64-debug
```

* **Launch Web Chapter 1:**
    This specific preset builds and launches the server for Chapter 1.

    ```bash
    cmake --build --preset launch-web-chapter1
    ```

### 4. Running Tests

To run tests, you can use a test preset.

**Syntax:**

```bash
ctest --preset <test-preset-name>
```

Alternatively, you can use the cmake build command with the `test` target.

**Syntax:**

```bash
cmake --build --preset <build-preset-name> --target test
```

**Examples:**

* **Windows (Debug):**
  
```bash
ctest --preset windows-x64-debug
```

* **Linux (Release):**

```bash
ctest --preset linux-x64-release
```

* **macOS (Apple Silicon, Debug):**

```bash
ctest --preset macos-arm64-debug
```

### 5. Packaging the Project

To create a package (e.g., an installer or an archive), use a package preset.

https://cmake.org/cmake/help/latest/cpack_gen/nsis.html
You HAVE TO install NSIS from http://nsis.sourceforge.net/
https://cmake.org/cmake/help/latest/cpack_gen/innosetup.html

//https://cmake.org/cmake/help/latest/cpack_gen/deb.html
//https://cmake.org/cmake/help/latest/cpack_gen/rpm.html

//https://cmake.org/cmake/help/latest/cpack_gen/bundle.html
//https://cmake.org/cmake/help/latest/cpack_gen/dmg.html
//https://cmake.org/cmake/help/latest/cpack_gen/productbuild.html

**Syntax:**

```bash
cmake --build --preset <package-preset-name> --target package
```

**Examples:**

* **Windows (Release):**

```bash
cmake --build --preset windows-x64-release --target package
```

* **Linux (Release):**

```bash
cmake --build --preset linux-x64-release --target package
```

* **macOS (Apple Silicon, Release):**

    ```bash
    cmake --build --preset macos-arm64-release --target package
    ```

### 6. Running Workflows

Workflow presets allow you to execute a sequence of steps (configure, build, test, package) with a single command.

**Syntax:**

```bash
cmake --workflow --preset <workflow-preset-name>
```

**Examples:**

* **Windows CI Release Workflow:**
    This will configure, build, test, and package the `windows-x64-release` target.

    ```bash
    cmake --workflow --preset windows-ci-release
    ```

* **Linux CI Release Workflow:**
    This will configure, build, test, and package the `linux-x64-release` target.

    ```bash
    cmake --workflow --preset linux-ci-release
    ```

* **macOS CI Release Workflow:**
    This will configure, build, test, and package the `macos-arm64-release` target.

    ```bash
    cmake --workflow --preset macos-ci-release
    ```

* **Configure, Build & Run Android (Chapter 2):**

    ```bash
    cmake --workflow --preset android-build-run-chapter02
    ```

* **Configure, Build & Launch Web (Chapter 2):**

    ```bash
    cmake --workflow --preset launch-chapter2-web
    ```
