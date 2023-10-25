#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <memory>
#include <vector>

using namespace llvm;

namespace {

class Profiler : public FunctionPass {
  std::unique_ptr<IRBuilder<>> Builder;

  auto &getOrCreateIRBuilder(LLVMContext &Ctx) {
    if (Builder)
      return *Builder;
    Builder = std::make_unique<IRBuilder<>>(Ctx);
    return *Builder;
  }

  bool isLoggerFunction(StringRef Name) {
    return Name == "logFuncStart";
  }

  bool isEngineFunction(StringRef Name) {
    return Name == "xorshift"  || Name == "changePosition" ||
           Name == "updateDot" || Name == "changeState" ||
           Name == "distance"  || Name == "getNearestDot" ||
           Name == "drawFrame" || Name == "initDots" ||
           Name == "app";
  }

  void generateBBPrologue(BasicBlock &BB) {
  }

  void generateBBEpilogue(BasicBlock &BB) {

  }

  void generateFuncPrologue(Function &F) {
    auto &Ctx = F.getContext();
    auto &IB = getOrCreateIRBuilder(Ctx);
    auto ParamTypes = ArrayRef<Type *>{IB.getInt8Ty()->getPointerTo()};
    auto RetType = Type::getVoidTy(Ctx);
    
    auto FuncType = FunctionType::get(RetType, ParamTypes, false);
    auto Func = 
      F.getParent()->getOrInsertFunction("logFuncStart", FuncType);
    auto FuncName = IB.CreateGlobalStringPtr(F.getName());
    auto Args = std::vector<Value *>{FuncName};
    
    IB.SetInsertPoint(&F.getEntryBlock().front());
    IB.CreateCall(Func, Args);
    outs() << "Inserted func prologue in " << F.getName() << "\n";
  }

  void generateFuncEpilogue(Function &F) {
    
  }

  void generateBinOp(Instruction &I) {

  }

  void generateCall(Instruction &I) {
    
  }

  void generateRet(Instruction &I) {
    
  }

  void generateCmp(Instruction &I) {
    
  }

  void runOnBB(BasicBlock &BB) {
    generateBBPrologue(BB);
    for (auto &I : BB) {
      if (auto *BinOp = dyn_cast<BinaryOperator>(&I))
        generateBinOp(I);
      if (auto *Call = dyn_cast<CallInst>(&I))
        generateCall(I);
      if (auto *Ret = dyn_cast<ReturnInst>(&I)) 
        generateRet(I);
      if (auto *Cmp = dyn_cast<CmpInst>(&I))
        generateCmp(I);
    }
    generateBBEpilogue(BB);
  }

public:
  static char ID;
  
  Profiler() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) override {
    if (isLoggerFunction(F.getName()) ||
        !isEngineFunction(F.getName()))
      return false;

    generateFuncPrologue(F);
    for (auto &BB : F)
      runOnBB(BB);
    generateFuncEpilogue(F);
    return true;
  }
};
} // namespace

char Profiler::ID = 0;

static void registerProfiler(const PassManagerBuilder &PMBuilder,
                                 legacy::PassManagerBase &PM) {
  PM.add(new Profiler());
}
static RegisterStandardPasses
  RegisterProfiler(PassManagerBuilder::EP_OptimizerLast,
                   registerProfiler);