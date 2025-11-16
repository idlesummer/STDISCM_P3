# Running a Visual Studio CMake Project in VSCode

A complete guide to set up and run a Visual Studio CMake project in Visual Studio Code with full debugging support.

---

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Install VSCode Extensions](#install-vscode-extensions)
3. [Fix CMakeLists.txt Issues](#fix-cmakeliststxt-issues)
4. [Configure VSCode Settings](#configure-vscode-settings)
5. [Open and Configure Project](#open-and-configure-project)
6. [Setup Debugging](#setup-debugging)
7. [Keyboard Shortcuts Reference](#keyboard-shortcuts-reference)
8. [Understanding Configure vs Build](#understanding-configure-vs-build)
9. [Troubleshooting](#troubleshooting)

---

## Prerequisites

### Required Software
- ✅ Windows 11 x64
- ✅ Visual Studio 2022 (with MSVC compiler)
- ✅ VSCode installed
- ✅ Existing CMake project from Visual Studio

### Hardware
- Intel CPU (x64 architecture)

---

## Install VSCode Extensions

Install these extensions from the Extensions marketplace (`Ctrl+Shift+X`):

1. **C/C++** (by Microsoft)
2. **CMake Tools** (by Microsoft)
3. **CMake** (by twxs) - for syntax highlighting

---

## Fix CMakeLists.txt Issues

### Fix 1: Correct the FetchContent Typo

**Line 42 - Change:**
```cmake
FetchCOntent_MakeAvailable(SFML)  # Wrong - capital 'O'
```

**To:**
```cmake
FetchContent_MakeAvailable(SFML)  # Correct - lowercase 'o'
```

### Fix 2: Add Compiler Guard for MSVC Settings

**Lines 6-7 - Change:**
```cmake
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
```

**To:**
```cmake
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
```

### Fix 3: Add Compile Commands Export (Optional but Recommended)

**Add after line 1:**
```cmake
cmake_minimum_required (VERSION 3.10)

# For better VSCode IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
```

### Fix 4: Keep FetchContent Caches Inside the Build Tree

When `FETCHCONTENT_BASE_DIR` points inside the source tree (e.g., `STDISCM_P2/thirdparty`),
FetchContent stores a `CMakeCache.txt` whose absolute paths include the original checkout
location. Copying the entire repo to a new folder (`STDISCM_P3`) reuses that cache and CMake
emits the error you saw. **Do not copy the `thirdparty/` folder between clones**—its cache is
only valid for the build directory that generated it. If you already copied it, delete the
folder so CMake can regenerate a clean cache:

```
CMake Error: The current CMakeCache.txt directory ... is different than the directory ...
```

Set the cache directory under the *build* tree instead so every out-of-source build gets its
own isolated third-party cache. Because the cache now lives under `build/`, you can freely copy
or rename the repository without dragging along stale third-party artifacts:

```cmake
set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/thirdparty CACHE PATH "Base Directory of Libs" FORCE)
```

If you already ran CMake with the old setting (or copied a `thirdparty/` folder from another
checkout), delete that folder—or the entire build directory—before reconfiguring.

### Complete Fixed CMakeLists.txt
```cmake
cmake_minimum_required (VERSION 3.10)

# For better VSCode IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add static runtimes (only when using MSVC)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("STDISCM_P2")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/thirdparty CACHE PATH "Base Directory of Libs" FORCE)
set(RESOURCES_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Media CACHE PATH "Base Directory of resources" FORCE)
set(PROJ_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src CACHE PATH "Project SRC" FORCE)

file(GLOB_RECURSE PROJ_SRC CONFIGURE_DEPENDS "${PROJ_SRC_PATH}/*.[ch]pp")

FetchContent_Declare(SFML
 GIT_REPOSITORY https://github.com/SFML/SFML.git
 GIT_TAG 2.6.x
 GIT_SHALLOW ON
 EXCLUDE_FROM_ALL
 SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable (STDISCM_P2 "${PROJ_SRC}" "STDISCM_P2.cpp" "STDISCM_P2.h")

target_compile_features(STDISCM_P2 PRIVATE cxx_std_20)

target_link_libraries(STDISCM_P2 PRIVATE sfml-graphics)

add_custom_command(
 TARGET STDISCM_P2 POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_SOURCE_DIR}/Media
   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Media
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
 set_property(TARGET STDISCM_P2 PROPERTY CXX_STANDARD 20)
endif()
```

---

## Configure VSCode Settings

Create or verify these files in your project's `.vscode` folder:

### Project Structure
```
YourProject/
├── .vscode/
│   ├── settings.json      ← VSCode behavior preferences
│   ├── tasks.json         ← Build tasks
│   └── launch.json        ← Debugging configuration
├── CMakePresets.json      ← Shared VS/VSCode configuration
├── CMakeLists.txt         ← Fixed CMake file
├── src/                   ← Your source files
└── Media/                 ← Assets folder
```

### `.vscode/settings.json`
```json
{
    "cmake.configureOnOpen": true,
    "cmake.automaticReconfigure": true,
    "cmake.options.statusBarVisibility": "compact",
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"
}
```

**What these settings do:**
- `configureOnOpen` - Automatically runs CMake configuration when opening project
- `automaticReconfigure` - Auto-reconfigures when CMakeLists.txt changes
- `statusBarVisibility` - Compact status bar (less clutter)
- `configurationProvider` - Better IntelliSense using CMake information

### `.vscode/tasks.json`
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "CMake Build",
            "type": "cmake",
            "command": "build",
            "problemMatcher": [],
            "detail": "CMake build task"
        }
    ]
}
```

**What this does:**
- Defines a build task that CMake can use
- Enables auto-build before debugging

### `.vscode/launch.json`
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(MSVC) Debug STDISCM_P2",
            "type": "cppvsdbg",
            "request": "launch",
            "program": "${workspaceFolder}/out/build/x64-debug/bin/STDISCM_P2.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}/out/build/x64-debug/bin",
            "environment": [],
            "console": "integratedTerminal",
            "preLaunchTask": "CMake Build"
        }
    ]
}
```

**What this does:**
- Configures F5 debugging
- Uses integrated terminal (avoids Windows Terminal errors)
- Auto-builds before debugging (`preLaunchTask`)

---

## Open and Configure Project

### Step 1: Open Project in VSCode
1. Open VSCode
2. `File → Open Folder`
3. Select your project folder (containing `CMakeLists.txt`)

### Step 2: Select Compiler Kit
1. Press `Ctrl+Shift+P`
2. Type: **"CMake: Select a Kit"**
3. Choose: **"Visual Studio Community 2022 Release - amd64"**
   - (Or your installed Visual Studio version)

### Step 3: Select Configuration Preset
1. Press `Ctrl+Shift+P`
2. Type: **"CMake: Configure"**
3. Choose: **"x64 Debug"**
   - For development/debugging
   - Use "x64 Release" for final builds

### Step 4: Wait for Configuration
CMake will:
- Download SFML from GitHub (~30 seconds)
- Configure the project (~1 minute)
- Generate build files

Expected output:
```
[cmake] -- The CXX compiler identification is MSVC 19.44...
[cmake] -- Configuring done (46.8s)
[cmake] -- Generating done (0.4s)
```

### Step 5: Build the Project
**Press `F7`** or:
- `Ctrl+Shift+P` → "CMake: Build"

First build takes longer (compiles SFML):
```
[build] [1/150] Building CXX object...
...
[build] [150/150] Linking CXX executable bin\STDISCM_P2.exe
[build] Build finished with exit code 0
```

### Step 6: Run the Project
**Press `Shift+F5`** (Run without debugging)

Or **Press `F5`** (Run with debugging - auto-builds first!)

Your executable runs from:
```
out/build/x64-debug/bin/STDISCM_P2.exe
```

---

## Setup Debugging

### Setting Breakpoints
1. Open any `.cpp` file (e.g., `BaseRunner.cpp`)
2. Click to the left of a line number
3. Red dot appears = breakpoint set

### Using the Debugger
1. **Press F5** - Starts debugging (auto-builds first)
2. When breakpoint hits:
   - Program pauses
   - Yellow arrow shows current line
   - Variables panel shows values

### Debug Controls
- **Continue (F5)** - Resume until next breakpoint
- **Step Over (F10)** - Execute current line
- **Step Into (F11)** - Go inside function
- **Step Out (Shift+F11)** - Exit current function
- **Restart (Ctrl+Shift+F5)** - Restart debugging
- **Stop (Shift+F5)** - Stop debugging

---

## Keyboard Shortcuts Reference

### Building and Running
| Action | Shortcut |
|--------|----------|
| **Build** | `F7` |
| **Run (no debug)** | `Shift+F5` |
| **Debug (auto-builds first)** | `F5` |
| **Stop** | `Shift+F5` (while running) |

### CMake Commands
| Action | Command |
|--------|---------|
| **Configure** | `Ctrl+Shift+P` → "CMake: Configure" |
| **Clean** | `Ctrl+Shift+P` → "CMake: Clean" |
| **Rebuild** | `Ctrl+Shift+P` → "CMake: Clean Rebuild" |
| **Select Kit** | `Ctrl+Shift+P` → "CMake: Select a Kit" |
| **Select Preset** | `Ctrl+Shift+P` → "CMake: Select Configure Preset" |

### Debugging
| Action | Shortcut |
|--------|----------|
| **Toggle Breakpoint** | `F9` |
| **Step Over** | `F10` |
| **Step Into** | `F11` |
| **Step Out** | `Shift+F11` |
| **Restart Debug** | `Ctrl+Shift+F5` |

### VSCode General
| Action | Shortcut |
|--------|----------|
| **Command Palette** | `Ctrl+Shift+P` |
| **Settings** | `Ctrl+,` |
| **Run & Debug Panel** | `Ctrl+Shift+D` |
| **Explorer** | `Ctrl+Shift+E` |
| **Terminal** | `Ctrl+\`` |

---

## Understanding Configure vs Build

### Configure Phase (Setup)
**What it does:** Prepares build instructions

**When it runs:**
- Opening VSCode (automatic)
- Editing CMakeLists.txt (automatic)
- Manually: `Ctrl+Shift+P` → "CMake: Configure"

**What it creates:**
- `out/build/x64-debug/CMakeCache.txt`
- `out/build/x64-debug/build.ninja`
- Build system files

**Duration:** Fast (~45 seconds)

**Does NOT create:** Your `.exe` file

**Output example:**
```
[cmake] -- Configuring done (46.8s)
[cmake] -- Generating done (0.4s)
```

### Build Phase (Compilation)
**What it does:** Compiles code into executable

**When it runs:**
- Pressing `F7`
- Pressing `F5` (auto-builds before debugging)
- Manually: `Ctrl+Shift+P` → "CMake: Build"

**What it creates:**
- `out/build/x64-debug/bin/STDISCM_P2.exe` ✅
- Object files (`.obj`)
- Libraries (`.lib`)

**Duration:** Slow (~5 minutes first time, ~30s incremental)

**Output example:**
```
[build] [1/150] Building CXX object...
[build] [150/150] Linking CXX executable bin\STDISCM_P2.exe
```

### Analogy
- **Configure** = Reading recipe and gathering ingredients 📋
- **Build** = Actually cooking the meal 🍳

---

## Troubleshooting

### Issue: "FetchContent_MakeAvailable not found"
**Cause:** Typo in CMakeLists.txt (capital 'O')  
**Solution:** Fix typo: `FetchContent_MakeAvailable` (lowercase 'o')

### Issue: "Cannot find SFML headers" or red squiggles
**Cause:** IntelliSense not configured  
**Solution:** 
1. Verify `settings.json` has: `"C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"`
2. Verify CMakeLists.txt has: `set(CMAKE_EXPORT_COMPILE_COMMANDS ON)`
3. Reconfigure: `Ctrl+Shift+P` → "CMake: Configure"

### Issue: F5 debugging shows error about Windows Terminal
**Cause:** `"console": "externalTerminal"` in launch.json  
**Solution:** Change to `"console": "integratedTerminal"` in launch.json

### Issue: F5 runs old code
**Cause:** Not building before debugging  
**Solution:** Add `"preLaunchTask": "CMake Build"` to launch.json

### Issue: "ninja: no work to do" but no .exe exists
**Cause:** Configured but never built  
**Solution:** Press `F7` to build

### Issue: CMake auto-runs every time I open VSCode
**This is normal!** It ensures your project is ready to build.  
**To disable:** Set `"cmake.configureOnOpen": false` in settings.json (not recommended)

### Issue: Build is very slow
**First build:** Normal (compiling SFML takes ~5 minutes)  
**Subsequent builds:** Should be fast (~30 seconds)  
**If always slow:** Clean and rebuild: `Ctrl+Shift+P` → "CMake: Clean Rebuild"

---

## Configuration Summary

### What You're Using
| Component | Value |
|-----------|-------|
| **IDE** | VSCode |
| **Compiler** | MSVC (Visual Studio 2022) |
| **Architecture** | x64 (64-bit) |
| **Build Type** | Debug (for development) |
| **Build System** | CMake + Ninja |
| **Libraries** | SFML 2.6.x (auto-downloaded) |

### Build Output Location
```
out/
└── build/
    └── x64-debug/
        ├── bin/
        │   ├── STDISCM_P2.exe      ← Your game
        │   └── Media/              ← Auto-copied assets
        └── thirdparty/
            └── sfml-build/         ← Compiled SFML
```

### Workflow Summary
1. **Open VSCode** → Auto-configure ✅
2. **Edit code** → Save
3. **Press F7** → Build 🔨
4. **Press F5** → Debug (auto-builds first) 🐛
5. **Press Shift+F5** → Run without debug ▶️

---

## Differences from Visual Studio

### Similarities
- ✅ Same compiler (MSVC)
- ✅ Same executable output
- ✅ Same CMakePresets.json
- ✅ Same build folders
- ✅ Both auto-configure on CMake changes

### Differences
- Build system: VS uses MSBuild, VSCode uses Ninja (both produce identical .exe)
- VSCode is lighter/faster
- Visual Studio has more advanced debugging features
- VSCode has better customization

### When to Use Each
- **Visual Studio:** Heavy debugging, profiling, complex diagnostics
- **VSCode:** Fast editing, quick testing, lightweight development

Both work perfectly! Switch between them seamlessly. ✅

---

## Your CMakePresets.json

If your project has `CMakePresets.json` (shared between VS and VSCode):
```json
{
    "version": 3,
    "configurePresets": [
        {
            "name": "windows-base",
            "hidden": true,
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/out/build/${presetName}",
            "installDir": "${sourceDir}/out/install/${presetName}",
            "cacheVariables": {
                "CMAKE_C_COMPILER": "cl.exe",
                "CMAKE_CXX_COMPILER": "cl.exe"
            },
            "condition": {
                "type": "equals",
                "lhs": "${hostSystemName}",
                "rhs": "Windows"
            }
        },
        {
            "name": "x64-debug",
            "displayName": "x64 Debug",
            "inherits": "windows-base",
            "architecture": {
                "value": "x64",
                "strategy": "external"
            },
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug"
            }
        },
        {
            "name": "x64-release",
            "displayName": "x64 Release",
            "inherits": "x64-debug",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Release"
            }
        }
    ]
}
```

**This ensures Visual Studio and VSCode use identical configurations!** ✅

---

## Notes

- First build takes ~5 minutes (SFML compilation)
- Subsequent builds are much faster (~30 seconds)
- SFML is downloaded once and cached
- The `Media` folder is automatically copied to output directory
- Your `.vscode` folder can be added to `.gitignore` (personal preferences)
- `CMakePresets.json` should be committed to Git (shared configuration)

---

## Success Checklist

- ✅ VSCode extensions installed
- ✅ CMakeLists.txt fixed (typo, MSVC guard, compile commands)
- ✅ `.vscode/settings.json` created
- ✅ `.vscode/tasks.json` created
- ✅ `.vscode/launch.json` created
- ✅ Kit selected (Visual Studio 2022 - amd64)
- ✅ Preset selected (x64 Debug)
- ✅ Project configured successfully
- ✅ Project builds successfully (F7)
- ✅ Project runs successfully (Shift+F5)
- ✅ Debugging works (F5 with breakpoints)

**You're all set! Happy coding! 🎮🚀**