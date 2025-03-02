cmake_minimum_required(VERSION 3.13)
project(Deserial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find MLIR from IREE build
find_package(MLIR REQUIRED CONFIG
  PATHS "C:/iree/iree/build/lib/cmake/mlir"
  NO_DEFAULT_PATH
)

set(IREE_BUILD_DIR "C:/iree/iree/build/llvm-project/lib")


# Include directories
include_directories(${MLIR_INCLUDE_DIRS})
include_directories("C:/iree/iree/third_party/llvm-project/mlir/include")
include_directories("C:/iree/iree/build/include")
link_directories(${IREE_BUILD_DIR})

# Define executable
add_executable(deserial deserial.cpp)
set_target_properties(deserial PROPERTIES FOLDER "Apps/deserial")
target_include_directories(deserial PRIVATE
    C:/iree/iree/third_party/llvm-project/mlir/include
    C:/iree/iree/build/include
)

# Link MLIR libraries
target_link_libraries(deserial PRIVATE
  MLIRIR
  MLIRParser
  MLIRSupport
  LLVMCore
  LLVMSupport
  MLIRBytecodeReader
  MLIRAsmParser
  MLIRFuncDialect
  MLIRTosaDialect
)

