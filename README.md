# ds_bars_test

Download llvm from source code, paste this in \llvm\clang-tools-extra\ folder, add in \llvm\clang-tools-extra\CmakeLists.txt "add_subdirectory(ds_bars_test)" and build it from CMake as part of llvm (cmake -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" ../llvm)

For correct run application add arguments ( for example in Visual Studio: Project Properties - Configuration Properties - Debugging - Command Arguments - "..\\..\\..\\..\\..\\..\clang-tools-extra\ds_bars_test\input\test.cpp ..\\..\\..\\..\\..\\..\clang-tools-extra\ds_bars_test\input\test2.cpp -- -w" )
