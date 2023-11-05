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
static FunctionCallee xorshiftCallee;
static FunctionCallee changePositionCallee;
static FunctionCallee updateDotCallee;
static StructType *Dot_t = nullptr;
static StructType *RGB_t = nullptr;

void xorShiftGen() {
  auto *FuncType = FunctionType::get(IB->getInt64Ty(), /*isVarArg*/ false);
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
  
  auto StoreAlign = MaybeAlign{8};
  auto *DotStruct = changePosition->getArg(0);
  auto *Const64i0 = ConstantInt::get(IB->getInt64Ty(), 0);
  auto *Const32i0 = ConstantInt::get(IB->getInt32Ty(), 0);
  auto *Const32i1 = ConstantInt::get(IB->getInt32Ty(), 1);
  auto *Const32i2 = ConstantInt::get(IB->getInt32Ty(), 2);
  auto *Const32i3 = ConstantInt::get(IB->getInt32Ty(), 3);
  auto *Const32i4 = ConstantInt::get(IB->getInt32Ty(), 4);
  auto *Const64i5 = ConstantInt::get(IB->getInt64Ty(), 5);
  auto *Const32i5 = ConstantInt::get(IB->getInt32Ty(), 5);
  auto *Const64i63 = ConstantInt::get(IB->getInt64Ty(), 63);

  auto Values = std::vector<Value *>{Const64i0, Const32i4};
  auto *Gep1 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(Const64i0, Gep1, StoreAlign);

  Values = std::vector<Value *>{Const64i0, Const32i2};
  auto *Gep2 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(Const64i5, Gep2, StoreAlign);

  Values = std::vector<Value *>{Const64i0, Const32i2};
  auto *Gep3 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(Const32i1, Gep3, StoreAlign);

  // not a tail call
  auto *XorShift1 = IB->CreateCall(xorshiftCallee);
  // not exact match
  auto *And1 = IB->CreateAnd(XorShift1, Const64i63);
  Values = std::vector<Value *>{Const64i0, Const32i0};
  auto *Gep4 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(And1, Gep4, StoreAlign);

  auto *XorShift2 = IB->CreateCall(xorshiftCallee);
  auto *And2 = IB->CreateAnd(XorShift2, Const64i63);
  Values = std::vector<Value *>{Const64i0, Const32i1};
  auto *Gep5 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(And2, Gep5, StoreAlign);

  auto *XorShift3 = IB->CreateCall(xorshiftCallee);
  auto *Trunc1 = IB->CreateTrunc(XorShift3, IB->getInt8Ty());
  Values = std::vector<Value *>{Const64i0, Const32i5, Const32i0};
  auto *Gep6 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(Trunc1, Gep6, StoreAlign);

  auto *XorShift4 = IB->CreateCall(xorshiftCallee);
  auto *Trunc2 = IB->CreateTrunc(XorShift4, IB->getInt8Ty());
  Values = std::vector<Value *>{Const64i0, Const32i5, Const32i1};
  auto *Gep7 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(Trunc2, Gep7, StoreAlign);

  auto *XorShift5 = IB->CreateCall(xorshiftCallee);
  auto *Trunc3 = IB->CreateTrunc(XorShift5, IB->getInt8Ty());
  Values = std::vector<Value *>{Const64i0, Const32i5, Const32i2};
  auto *Gep8 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(Trunc3, Gep8, StoreAlign);

  IB->CreateRetVoid();
}

void updateDotGen() {
  auto *FuncType = FunctionType::get(IB->getVoidTy(), 
                                     PointerType::getUnqual(Dot_t), 
                                     /*isVarArg*/ false);
  auto *changePosition =
    Function::Create(FuncType, Function::ExternalLinkage, "updateDot", *M);
  auto *Entry = BasicBlock::Create(*Ctx, "Entry", changePosition);
  auto *BB1 = BasicBlock::Create(*Ctx, "BB1", changePosition);
  auto *BB2 = BasicBlock::Create(*Ctx, "BB2", changePosition);
  auto *BB3 = BasicBlock::Create(*Ctx, "BB3", changePosition);
  auto *BB4 = BasicBlock::Create(*Ctx, "BB4", changePosition);
  auto *BB5 = BasicBlock::Create(*Ctx, "BB5", changePosition);
  auto *BBRet = BasicBlock::Create(*Ctx, "BBRet", changePosition);
  IB->SetInsertPoint(Entry);

  auto LoadAlign = MaybeAlign{8};
  auto *DotStruct = changePosition->getArg(0);
  auto *Const32ineg1 = ConstantInt::get(IB->getInt32Ty(), -1);
  auto *Const64i0 = ConstantInt::get(IB->getInt64Ty(), 0);
  auto *Const32i2 = ConstantInt::get(IB->getInt32Ty(), 2);
  auto *Const32i3 = ConstantInt::get(IB->getInt32Ty(), 3);
  auto *Const32i4 = ConstantInt::get(IB->getInt32Ty(), 4);
  auto *Const64i64 = ConstantInt::get(IB->getInt64Ty(), 64);

  auto Values = std::vector<Value *>{Const64i0, Const32i3};
  auto *Gep1 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  auto *Load1 = IB->CreateAlignedLoad(IB->getInt32Ty(), Gep1, LoadAlign);
  
  auto *Icmp1 = IB->CreateICmpEQ(Load1, Const32ineg1);

  Values = std::vector<Value *>{Const64i0, Const32i4};
  auto *Gep2 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  auto *Load2 = IB->CreateAlignedLoad(IB->getInt64Ty(), Gep2, LoadAlign);

  IB->CreateCondBr(Icmp1, BB1, BB3);
  IB->SetInsertPoint(BB1);

  Values = std::vector<Value *>{Const64i0, Const32i2};
  auto *Gep3 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  auto *Load3 = IB->CreateAlignedLoad(IB->getInt64Ty(), Gep3, LoadAlign);

  auto *Icmp2 = IB->CreateICmpUGT(Load2, Load3);

  IB->CreateCondBr(Icmp2, BB3, BB2);
  IB->SetInsertPoint(BB2);

  auto Args = std::vector<Value *>{DotStruct};
  IB->CreateCall(changePositionCallee, Args);

  IB->CreateBr(BBRet);
  IB->SetInsertPoint(BB3);

  Values = std::vector<Value *>{Const64i0, Const32i4};
  auto *Gep4 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);

  auto *Icmp3 = IB->CreateICmpUGT(Load2, Const64i64);

  IB->CreateCondBr(Icmp3, BB4, BB5);
  IB->SetInsertPoint(BB4);

  IB->CreateAlignedStore(Const32ineg1, Gep1, LoadAlign);

  IB->CreateBr(BB5);
  IB->SetInsertPoint(BB5);

  auto *Phi = IB->CreatePHI(IB->getInt32Ty(), 2);
  Phi->addIncoming(Const32ineg1, BB4);
  Phi->addIncoming(Load1, Entry);

  Values = std::vector<Value *>{Const64i0, Const32i2};
  auto *Gep5 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  auto *Load4 = IB->CreateAlignedLoad(IB->getInt64Ty(), Gep5, LoadAlign);

  auto *Sext = IB->CreateSExt(Phi, IB->getInt64Ty());
  auto *Mul = IB->CreateMul(Load4, Sext);
  auto *Add = IB->CreateAdd(Mul, Load2);
  IB->CreateAlignedStore(Add, Gep4, LoadAlign);

  IB->CreateBr(BBRet);
  IB->SetInsertPoint(BBRet);

  IB->CreateRetVoid();
}

void changeStateGen() {
  auto *FuncType = FunctionType::get(IB->getVoidTy(), 
                                     PointerType::getUnqual(Dot_t), 
                                     /*isVarArg*/ false);
  auto *changeState =
    Function::Create(FuncType, Function::ExternalLinkage, "changeState", *M);
  auto *Entry = BasicBlock::Create(*Ctx, "Entry", changeState);
  auto *BB1 = BasicBlock::Create(*Ctx, "BB1", changeState);
  auto *BB2 = BasicBlock::Create(*Ctx, "BB2", changeState);
  IB->SetInsertPoint(Entry);

  auto *DotStruct = changeState->getArg(0);
  auto *Const64i0 = ConstantInt::get(IB->getInt64Ty(), 0);
  auto *Const64i1 = ConstantInt::get(IB->getInt64Ty(), 1);
  auto *Const64i10 = ConstantInt::get(IB->getInt64Ty(), 10);

  IB->CreateBr(BB2);
  IB->SetInsertPoint(BB1);

  IB->CreateRetVoid();
  IB->SetInsertPoint(BB2);

  auto *Phi = IB->CreatePHI(IB->getInt64Ty(), 2);

  auto Values = std::vector<Value *>{Const64i0};
  auto *Gep = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  auto Args = std::vector<Value *>{Gep};
  IB->CreateCall(updateDotCallee, Args);

  // nuw - no unsigned wrap
  // nsw - no signed wrap
  auto *Add = IB->CreateAdd(Phi, Const64i1);
  auto *Icmp = IB->CreateICmpEQ(Add, Const64i10);

  IB->CreateCondBr(Icmp, BB1, BB2);

  // нужно чтобы разнести определение и реализацию
  Phi->addIncoming(Const64i0, Entry);
  Phi->addIncoming(Add, BB2);
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

void createCallees() {
  assert(Dot_t && RGB_t);
  auto *XorShift_t =
    FunctionType::get(IB->getInt64Ty(), /*isVarArg*/ false);
  xorshiftCallee = M->getOrInsertFunction("xorshift", XorShift_t);

  auto *ChangePosition_t = FunctionType::get(IB->getVoidTy(), 
                                             PointerType::getUnqual(Dot_t),
                                             /*isVarArg*/ false);
  changePositionCallee = 
    M->getOrInsertFunction("changePosition", ChangePosition_t);

  auto *UpdateDot_t = FunctionType::get(IB->getVoidTy(), 
                                     PointerType::getUnqual(Dot_t), 
                                     /*isVarArg*/ false);
  updateDotCallee = 
     M->getOrInsertFunction("updateDot", UpdateDot_t);
}

int main() {
  Ctx = std::make_unique<LLVMContext>();
  M = std::make_unique<Module>("Handmade-IR", *Ctx);
  IB = std::make_unique<IRBuilder<>>(*Ctx);
  createStructs();
  createCallees();
  assert(RGB_t && Dot_t);

  xorShiftGen();
  changePositionGen();
  updateDotGen();
  changeStateGen();
  
  M->print(outs(), nullptr);
}