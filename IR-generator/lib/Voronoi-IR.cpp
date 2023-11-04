#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

#include <memory>
#include <vector>
#include <cassert>

using namespace llvm;

static std::unique_ptr<LLVMContext> Ctx;
static std::unique_ptr<Module> M;
static std::unique_ptr<IRBuilder<>> IB;
static StructType *Dot_t = nullptr;
static StructType *RGB_t = nullptr;

void xorShiftGen() {
  auto *FuncType = FunctionType::get(IB->getVoidTy(), /*isVarArg*/ false);
  auto *xorshift = 
    Function::Create(FuncType, Function::ExternalLinkage, "xorshift", *M);
  auto *BB = BasicBlock::Create(*Ctx, "Entry", xorshift);
  IB->SetInsertPoint(BB);

  auto *SeedT = IB->getInt64Ty();
  auto SeedVal = APInt{/*NumBits*/ 64, 1ull};
  auto SeedAlign = MaybeAlign{8};
  auto *GVSeed = 
    new GlobalVariable(*M, SeedT,
                      /*isConstant*/false,
                      /*Linkage*/GlobalValue::CommonLinkage,
                      /*Initializer*/ Constant::getIntegerValue(SeedT, SeedVal),
                      /*Name*/ "Seed");
  GVSeed->setAlignment(SeedAlign);
  auto *Const13 = ConstantInt::get(IB->getInt64Ty(), 13);
  auto *Const7 = ConstantInt::get(IB->getInt64Ty(), 7);
  auto *Const17 = ConstantInt::get(IB->getInt64Ty(), 17);

  auto *LoadInstr = 
    IB->CreateAlignedLoad(SeedT, GVSeed, SeedAlign, /*isVolatile*/ false);
  auto *Shl1 = IB->CreateShl(LoadInstr, Const13);
  auto *Xor1 = IB->CreateXor(Shl1, LoadInstr);
  auto *Lshr = IB->CreateLShr(Xor1, Const7);
  auto *Xor2 = IB->CreateXor(Lshr, Xor1);
  auto *Shl2 = IB->CreateShl(Xor2, Const17);
  auto *Xor3 = IB->CreateXor(Shl2, Xor2);
  IB->CreateAlignedStore(Xor3, GVSeed, SeedAlign);

  IB->CreateRet(Xor3);
}

void changePositionGen() {
  auto *FuncType = FunctionType::get(IB->getVoidTy(), 
                                     PointerType::getUnqual(Dot_t), 
                                     /*isVarArg*/ false);
  auto *changePosition =
    Function::Create(FuncType, Function::ExternalLinkage, "changePosition", *M);
  auto *BB = BasicBlock::Create(*Ctx, "Entry", changePosition);
  IB->SetInsertPoint(BB);
  
  auto *Const0 = ConstantInt::get(IB->getInt64Ty(), 0);
  auto *Const4 = ConstantInt::get(IB->getInt32Ty(), 4);


  auto Vlaues = std::vector<Value *>{Const0, Const4};
  auto *Gep1 = IB->CreateInBoundsGEP(Dot_t, changePosition->getArg(0), Vlaues);
}

void createStructs() {
  auto RGBElements = std::vector<Type *>{IB->getInt8Ty(),
                                         IB->getInt8Ty(),
                                         IB->getInt8Ty()};
  RGB_t = StructType::create(*Ctx, RGBElements, "RGB");

  auto DotElements = std::vector<Type *>{IB->getInt64Ty(),
                                         IB->getInt64Ty(),
                                         IB->getInt64Ty(),
                                         IB->getInt32Ty(),
                                         IB->getInt64Ty(),
                                         RGB_t};
  Dot_t = StructType::create(*Ctx, DotElements, "Dot");
}

int main() {
  Ctx = std::make_unique<LLVMContext>();
  M = std::make_unique<Module>("Handmade-IR", *Ctx);
  IB = std::make_unique<IRBuilder<>>(*Ctx);
  createStructs();
  assert(RGB_t && Dot_t);

  xorShiftGen();
  changePositionGen();
  
  M->print(outs(), nullptr);
}