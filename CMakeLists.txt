cmake_minimum_required(VERSION 3.10)

project(ds_bars_test)

#find_package(Clang REQUIRED CONFIG)

#add_executable(ds_bars_test main.cpp)

#target_compile_features(ds_bars_test PRIVATE cxx_std_17)

#target_include_directories(ds_bars_test PRIVATE ${CLANG_INCLUDE_DIRS})

#target_link_libraries(ds_bars_test PRIVATE clangTooling)

set(LLVM_LINK_COMPONENTS support)

add_clang_executable(ds_bars_test main.cpp)

target_link_libraries(ds_bars_test
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangSerialization
  clangTooling
  )