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
           Name == "app";
  }

  // Логирование базовых блоков выдает ошибку на этапе перевода в MIR

  void generateBinOp(Instruction &I) {
    assert(I.getParent());
    auto &BB = *I.getParent();
    auto &Ctx = BB.getContext();
    auto &IB = getOrCreateIRBuilder(Ctx);
    auto ParamTypes = ArrayRef<Type *>{/*Op*/  Type::getInt32Ty(Ctx),
                                       /*Lhs*/ Type::getInt32Ty(Ctx),
                                       /*Rhs*/ Type::getInt32Ty(Ctx),
                                       /*Op name*/ IB.getInt8Ty()->getPointerTo(),
                                       /*Instr Id*/ Type::getInt64Ty(Ctx)};
    auto RetType = Type::getVoidTy(Ctx);
    auto FuncType = FunctionType::get(RetType, ParamTypes, false);
    auto Func = 
      BB.getParent()->getParent()->getOrInsertFunction("logBinOp", FuncType);

    IB.SetInsertPoint(&I);
    // Вставлем после инструкции?
    IB.SetInsertPoint(&BB, ++IB.GetInsertPoint());
    auto *Op = dyn_cast<BinaryOperator>(&I);
    assert(Op);
    auto *Lhs = Op->getOperand(0);
    auto *Rhs = Op->getOperand(1); 
    auto *OpName = IB.CreateGlobalStringPtr(Op->getOpcodeName());
    auto *InstrId = 
      ConstantInt::get(IB.getInt64Ty(), reinterpret_cast<uint64_t>(&I));
    auto Args = std::vector<Value *>{Op, Lhs, Rhs, OpName, InstrId};
    IB.CreateCall(Func, Args);
  }

  void generateCall(Instruction &I) {
    assert(I.getParent());
    assert(I.getParent()->getParent());
    auto &F = *I.getParent()->getParent();
    auto &Ctx = F.getContext();
    auto &IB = getOrCreateIRBuilder(Ctx);
    auto ParamTypes = ArrayRef<Type *>{IB.getInt8Ty()->getPointerTo()};
    auto RetType = Type::getVoidTy(Ctx);
    
    auto *Call = dyn_cast<CallInst>(&I);
    assert(Call);
    assert(Call->getCalledFunction());
    auto CalleName = Call->getCalledFunction()->getName();
    
    if (isLoggerFunction(CalleName))
      return;

    auto FuncType = FunctionType::get(RetType, ParamTypes, false);
    auto LogFuncName = isEngineFunc(CalleName) ? StringRef{"logFuncStart"} 
                                               : StringRef{"logCallOp"};
    auto Func = 
      F.getParent()->getOrInsertFunction(LogFuncName, FuncType);
    
    /* !!! Надо сначала делать SetInsertPoint а потом CreateGlobalStringPtr !!!*/
    IB.SetInsertPoint(&I);
    auto *FuncName = IB.CreateGlobalStringPtr(CalleName);
    auto Args = std::vector<Value *>{FuncName};
    IB.CreateCall(Func, Args);
  }

  void generateRet(Instruction &I) {
    assert(I.getParent());
    assert(I.getParent()->getParent());
    auto &F = *I.getParent()->getParent();
    auto &Ctx = F.getContext();
    auto &IB = getOrCreateIRBuilder(Ctx);
    auto ParamTypes = ArrayRef<Type *>{IB.getInt8Ty()->getPointerTo()};
    auto RetType = Type::getVoidTy(Ctx);
    
    auto FuncType = FunctionType::get(RetType, ParamTypes, false);
    auto Func = 
      F.getParent()->getOrInsertFunction("logFuncEnd", FuncType);
    
    IB.SetInsertPoint(&I);
    auto *FuncName = IB.CreateGlobalStringPtr("ret");
    auto Args = std::vector<Value *>{FuncName};
    IB.CreateCall(Func, Args);
  }

  void generateCmp(Instruction &I) {
    return;
    assert(I.getParent());
    auto &BB = *I.getParent();
    auto &Ctx = BB.getContext();
    auto &IB = getOrCreateIRBuilder(Ctx);
    auto ParamTypes = ArrayRef<Type *>{/*Op*/  Type::getInt32Ty(Ctx),
                                       /*Predicate*/ Type::getInt32Ty(Ctx),
                                       /*Op name*/ IB.getInt8Ty()->getPointerTo(),
                                       /*Instr Id*/ Type::getInt64Ty(Ctx)};
    auto RetType = Type::getVoidTy(Ctx);
    
    auto FuncType = FunctionType::get(RetType, ParamTypes, false);
    auto Func = 
      BB.getParent()->getParent()->getOrInsertFunction("logCmpOp", FuncType);

    IB.SetInsertPoint(&I);
    auto *Op = dyn_cast<CmpInst>(&I);
    assert(Op);
    auto Pred = ConstantInt::get(IB.getInt32Ty(), Op->getPredicate()); 
    auto *OpName = IB.CreateGlobalStringPtr(Op->getOpcodeName());
    auto *InstrId = 
      ConstantInt::get(IB.getInt64Ty(), reinterpret_cast<uint64_t>(&I));
    auto Args = std::vector<Value *>{Op, Pred, OpName, InstrId};
    IB.CreateCall(Func, Args);
  }

  void runOnBB(BasicBlock &BB) {
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