# Simulation Volumetric Sandbox

Simulation Volumetric Sandbox is a modular, cross-platform C++17 engine scaffold built for advanced graphics and audio simulations. The project features a unified driver architecture supporting multiple rendering backends (Vulkan, DirectX, Metal, and OpenGL) alongside a dynamic audio system powered by `miniaudio`.

It utilizes modern CMake presets for seamless building across Windows, Linux, and macOS, including optional support for Linux-to-Windows cross-compilation via MinGW-w64 and native Objective-C++ (Cocoa/GNUstep) integration.

## Requirements

Install these on every platform:

- CMake 3.22 or newer
- Git
- A C++17 compiler
- Vulkan SDK or Vulkan development headers and loader
- GPU driver with Vulkan support

The project can fetch GLM and Dear ImGui automatically when `WR_FETCH_DEPS=ON`, which is enabled by the presets.

## Windows Setup

Install:

- Visual Studio 2026 or Build Tools with the Desktop C++ workload
- Windows SDK
- Vulkan SDK from LunarG
- Ninja, if you want `compile_commands.json` for clangd

Check Vulkan SDK:

```powershell
$env:VULKAN_SDK
```

Configure and build with Visual Studio:

```powershell
cmake --preset windows-x64-vs2026-debug
cmake --build --preset windows-x64-vs2026-debug
```

Release build:

```powershell
cmake --preset windows-x64-vs2026-release
cmake --build --preset windows-x64-vs2026-release
```

For Ninja on Windows, run the commands from a Visual Studio Developer PowerShell or x64 Native Tools shell:

```powershell
cmake --preset windows-x64-ninja-debug
cmake --build --preset windows-x64-ninja-debug
```

## Linux Setup

Install build tools and Vulkan development packages.

Ubuntu or Debian:

```bash
sudo apt update
sudo apt install build-essential cmake ninja-build git libvulkan-dev vulkan-tools libx11-dev libgl1-mesa-dev
```

Fedora:

```bash
sudo dnf install gcc-c++ cmake ninja-build git vulkan-devel vulkan-tools libX11-devel mesa-libGL-devel
```

Arch Linux:

```bash
sudo pacman -S base-devel cmake ninja git vulkan-headers vulkan-loader vulkan-tools libx11 mesa
```

### GNUstep Setup (Optional for Cocoa/Objective-C++ on Linux)

If you are experimenting with Cocoa (`.mm`) compilation on Linux, you must install GNUstep and its GUI dependencies.

**Ubuntu or Debian:**
```bash
sudo apt install gnustep gnustep-devel libgnustep-base-dev libgnustep-gui-dev gobjc++
```

**Arch Linux (AUR):**
```bash
paru -S gnustep-make gnustep-base gnustep-gui gnustep-back
```

#### Troubleshooting GNUstep Installation (Arch Linux / AUR)

When installing GNUstep and its dependencies from the AUR (like `gnustep-gui` or `gnustep-back`), you may encounter conflicts and build errors due to older dependencies such as `libpng` and `mozjpeg`.

**1. `mozjpeg` vs `libjpeg-turbo` Conflict**
If you are prompted with a conflict between `mozjpeg` and `libjpeg-turbo` when running `paru -S mozjpeg`, you have two options:
- **Recommended**: Install `mozjpeg-git` instead, which often resolves outdated CMake and dependency issues automatically:
  ```bash
  paru -S mozjpeg-git
  ```

**2. `mozjpeg` CMake Build Error (makepkg)**
If you still want to install `mozjpeg` (non-git) and encounter a CMake error during `build()` saying `Compatibility with CMake < 3.5 has been removed`, you must manually edit the PKGBUILD:
- Run `paru -S mozjpeg`
- When prompted `:: Proceed to review? [Y/n]:`, press **Y**.
- Find the `build()` function and the `cmake` command.
- Add the `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` argument to the cmake command. For example:
  ```bash
  cmake -B build -S "$srcdir/$pkgname-$pkgver" \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
        ...
  ```
- Save the file and close the editor to continue the build.

Configure and build:

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
```

Release build:

```bash
cmake --preset linux-release
cmake --build --preset linux-release
```

## macOS Setup

Install:

- Xcode Command Line Tools
- CMake
- Ninja
- Vulkan SDK with MoltenVK

With Homebrew for common tools:

```bash
xcode-select --install
brew install cmake ninja git
```

Install the Vulkan SDK from LunarG, then load its environment for the current shell. Replace the SDK path with your installed version:

```bash
source "$HOME/VulkanSDK/<version>/setup-env.sh"
```

Configure and build:

```bash
cmake --preset macos-debug
cmake --build --preset macos-debug
```

Release build:

```bash
cmake --preset macos-release
cmake --build --preset macos-release
```

## Linux to Windows Cross Build

The MinGW presets are optional and intended for building a Windows executable from Linux.

Install MinGW-w64 and Windows Vulkan development files. Package names vary by distro, but the compiler triplet expected by the toolchain is:

```text
x86_64-w64-mingw32-gcc
x86_64-w64-mingw32-g++
x86_64-w64-mingw32-windres
```

Configure and build:

```bash
cmake --preset linux-mingw-debug
cmake --build --preset linux-mingw-debug
```

Release build:

```bash
cmake --preset linux-mingw-release
cmake --build --preset linux-mingw-release
```

If CMake cannot find the Windows Vulkan loader during cross compilation, provide the include and library paths explicitly:

```bash
cmake --preset linux-mingw-debug \
  -DVulkan_INCLUDE_DIR=/path/to/windows-vulkan/include \
  -DVulkan_LIBRARY=/path/to/windows-vulkan/lib/vulkan-1.lib
```

## Available Presets

List presets:

```bash
cmake --list-presets
cmake --build --list-presets
```

Configure presets:

| Preset | Platform | Generator | Build type |
| --- | --- | --- | --- |
| `windows-x64-vs2026-debug` | Windows | Visual Studio 18 2026 | Debug |
| `windows-x64-vs2026-release` | Windows | Visual Studio 18 2026 | Release |
| `windows-x64-ninja-debug` | Windows | Ninja | Debug |
| `windows-x64-ninja-release` | Windows | Ninja | Release |
| `linux-debug` | Linux | Ninja | Debug |
| `linux-release` | Linux | Ninja | Release |
| `linux-mingw-debug` | Linux to Windows | Ninja + MinGW-w64 | Debug |
| `linux-mingw-release` | Linux to Windows | Ninja + MinGW-w64 | Release |
| `macos-debug` | macOS | Ninja | Debug |
| `macos-release` | macOS | Ninja | Release |

## clangd and Vulkan IntelliSense

clangd needs `compile_commands.json` to know Vulkan include paths, platform defines, compiler flags, and dependency include directories. The Visual Studio generator does not produce this file, even when `CMAKE_EXPORT_COMPILE_COMMANDS=ON`.

Use a Ninja preset to generate the compile database, then expose it at the repository root.

Windows:

```powershell
cmake --preset windows-x64-ninja-debug
Copy-Item build/windows-x64-ninja-debug/compile_commands.json compile_commands.json
```

Linux:

```bash
cmake --preset linux-debug
ln -sf build/linux-debug/compile_commands.json compile_commands.json
```

macOS:

```bash
cmake --preset macos-debug
ln -sf build/macos-debug/compile_commands.json compile_commands.json
```

After that, restart clangd or reload your editor window.

If clangd still cannot find `vulkan/vulkan.h`:

- Windows: confirm `VULKAN_SDK` is set and use the Ninja preset from a Visual Studio Developer shell.
- Linux: confirm `libvulkan-dev`, `vulkan-devel`, or equivalent headers are installed.
- macOS: confirm the Vulkan SDK environment script has been sourced before configuring.

## CMake Options

| Option | Default in presets | Description |
| --- | --- | --- |
| `WR_BUILD_APP` | `ON` | Build the application target. |
| `WR_FETCH_DEPS` | `ON` | Fetch GLM and Dear ImGui when they are not installed. |
| `WR_COMPILE_SHADER` | `OFF` | Reserved for shader compilation flow. |
| `WR_WIN32_EXECUTABLE` | `ON` | Build Windows target with the GUI subsystem. |
| `WR_DEBUG_CONSOLE` | `ON` for debug, `OFF` for release | Open a debug console on Windows builds. |

## Output

By default, runtime binaries are written to the repository root build folders, such as:

```text
Debug/vk_volumetric.exe
Release/vk_volumetric.exe
```

The CMake build trees live under:

```text
build/<preset-name>
```

## Troubleshooting

If CMake cannot find Vulkan, install the Vulkan SDK or development package and reconfigure the preset from a fresh shell.

If clangd sees standard C++ but not Vulkan symbols, regenerate `compile_commands.json` with a Ninja preset and copy or symlink it to the repository root.

If Windows MSBuild fails with a `Path` and `PATH` duplicate environment error, normalize the current PowerShell process and build again:

```powershell
$pathValue = [Environment]::GetEnvironmentVariable('Path', 'Process')
[Environment]::SetEnvironmentVariable('PATH', $null, 'Process')
[Environment]::SetEnvironmentVariable('Path', $pathValue, 'Process')
cmake --build --preset windows-x64-vs2026-debug
```

If a dependency fetch fails, check network access or install the dependency system-wide, then configure with `WR_FETCH_DEPS=OFF`.
