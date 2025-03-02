#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "mlir/IR/AsmState.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinDialect.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"  // For func::FuncDialect
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Parser/Parser.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"

using namespace mlir;

OwningOpRef<ModuleOp> deserializeMLIRBytecode(const std::string& filepath, MLIRContext* context) {
  std::ifstream file(filepath, std::ios::binary);
  if (!file) {
    std::cerr << "Error: Could not open file: " << filepath << std::endl;
    return nullptr;
  }

  file.seekg(0, std::ios::end);
  size_t size = file.tellg();
  std::string buffer(size, ' ');
  file.seekg(0);
  file.read(&buffer[0], size);
  file.close();

  std::unique_ptr<llvm::MemoryBuffer> memBuffer = llvm::MemoryBuffer::getMemBuffer(
      llvm::StringRef(buffer.data(), buffer.size()), filepath, false);

  llvm::SourceMgr sourceMgr;
  sourceMgr.AddNewSourceBuffer(std::move(memBuffer), llvm::SMLoc());

  OwningOpRef<ModuleOp> module = parseSourceFile<ModuleOp>(sourceMgr, ParserConfig(context));
  if (!module) {
    std::cerr << "Error: Failed to parse MLIR bytecode from " << filepath << std::endl;
    return nullptr;
  }

  if (failed(verify(*module))) {
    std::cerr << "Error: Module verification failed" << std::endl;
    module->dump();
    return nullptr;
  }

  return module;
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <path-to-mlirbc-file>" << std::endl;
    return 1;
  }

  std::string filepath = argv[1];
  MLIRContext context;
  context.getOrLoadDialect<BuiltinDialect>();
  context.getOrLoadDialect<func::FuncDialect>();  // Use func::FuncDialect instead of FuncDialect

  OwningOpRef<ModuleOp> module = deserializeMLIRBytecode(filepath, &context);
  if (!module) {
    return 1;
  }

  std::cout << "Successfully deserialized module:\n";
  module->print(llvm::outs());

  return 0;
}