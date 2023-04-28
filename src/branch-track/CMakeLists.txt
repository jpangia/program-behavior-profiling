set(LLVM_LINK_COMPONENTS support)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE) 

add_clang_executable(branch-track
  BranchTrack.cpp
  )
target_link_libraries(branch-track
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangSerialization
  clangTooling
  clangTransformer
  )
