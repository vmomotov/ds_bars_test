cmake_minimum_required(VERSION 3.10)

project(ds_bars_test)

set(LLVM_LINK_COMPONENTS support)

add_clang_executable(ds_bars_test 
	main.cpp
	CodeRecordsSingleton.cpp
	)

target_include_directories(ds_bars_test 
  PRIVATE
  includes/
  )
  
target_link_libraries(ds_bars_test
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangSerialization
  clangTooling
  )