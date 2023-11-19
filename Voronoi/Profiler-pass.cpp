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
#include <cassert>

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
    return Name == "logFuncStart" ||  Name == "logFuncEnd" ||
           Name == "logBinOp" || Name == "logCallOp" ||
           Name == "logCmpOp";
  }

  bool isEngineFunc(StringRef Name) {
    return Name == "changePosition" || Name == "updateDot" ||
           Name == "changeState" || Name == "getNearestDot" ||
           Name == "drawFrame" || Name == "initDots" ||
           Name == "distance" || Name == "app";
  }

  bool isUnsupportedInstr(Instruction &I) {
    return dyn_cast<PHINode>(&I);
  }
  
  void makeStatLog(Instruction &I) {
    dbgs() << reinterpret_cast<uint64_t>(&I) << " " 
           << I.getOpcodeName() << "\n";
  }

  void makeDynLog(Instruction &I) {
    if (isUnsupportedInstr(I))
      return;

    assert(I.getParent());
    auto &BB = *I.getParent();
    auto &Ctx = BB.getContext();
    auto &IB = getOrCreateIRBuilder(Ctx);
    auto ParamTypes = ArrayRef<Type *>{/*Instr Id*/ Type::getInt64Ty(Ctx)};
    auto RetType = Type::getVoidTy(Ctx);
    
    IB.SetInsertPoint(&I);
    auto FuncType = FunctionType::get(RetType, ParamTypes, false);
    auto Func = 
      BB.getParent()->getParent()->getOrInsertFunction("logInstr", FuncType);
    auto *InstrId = 
      ConstantInt::get(IB.getInt64Ty(), reinterpret_cast<uint64_t>(&I));
    assert(InstrId);
    IB.CreateCall(Func, InstrId);
  }

  void runOnBB(BasicBlock &BB) {
    for (auto &I : BB) {
      makeDynLog(I);
      makeStatLog(I);      
    }
  }

public:
  static char ID;
  
  Profiler() : FunctionPass(ID) {}

  virtual bool runOnFunction(Function &F) override {
    if (isLoggerFunction(F.getName()) ||
        !isEngineFunc(F.getName()))
      return false;

    for (auto &BB : F)
      runOnBB(BB);
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