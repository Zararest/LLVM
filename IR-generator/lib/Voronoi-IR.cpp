#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

#include "Graphics.h"

#include <memory>
#include <vector>
#include <cassert>

using namespace llvm;

static std::unique_ptr<LLVMContext> Ctx;
static std::unique_ptr<Module> M;
static std::unique_ptr<IRBuilder<>> IB;
static Function *xorshift = nullptr;
static Function *changePosition = nullptr;
static Function *updateDot = nullptr;
static Function *changeState = nullptr;
static Function *distance = nullptr;
static Function *getNearestDot = nullptr;
static Function *drawFrame = nullptr;
static Function *initDots = nullptr;
static Function *app = nullptr;
static FunctionCallee simPutPixel;
static FunctionCallee simFlush;
static StructType *Dot_t = nullptr;
static StructType *RGB_t = nullptr;

auto StoreAlign = MaybeAlign{8};
auto LoadAlign = MaybeAlign{8};
IntegerType *Int24_t = nullptr;
PointerType *Int24Ptr = nullptr; 
PointerType *DotPtr = nullptr;
ConstantPointerNull *DotNullptr = nullptr;
ConstantInt *Const32ineg1 = nullptr;
ConstantInt *Const64i0 = nullptr;
ConstantInt *Const32i0 = nullptr;
ConstantInt *Const64i1 = nullptr;
ConstantInt *Const32i1 = nullptr;
ConstantInt *Const32i2 = nullptr;
ConstantInt *Const32i3 = nullptr;
ConstantInt *Const32i4 = nullptr;
ConstantInt *Const64i5 = nullptr;
ConstantInt *Const32i5 = nullptr;
ConstantInt *Const64i10 = nullptr;
ConstantInt *Const64i63 = nullptr;
ConstantInt *Const64i64 = nullptr;
ConstantInt *Const64i48830 = nullptr;
ConstantInt *Const24i657930 = nullptr;

void xorShiftGen() {
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
  auto *BB = BasicBlock::Create(*Ctx, "Entry", changePosition);
  IB->SetInsertPoint(BB);
  
  auto *DotStruct = changePosition->getArg(0);

  auto Values = std::vector<Value *>{Const64i0, Const32i4};
  auto *Gep1 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(Const64i0, Gep1, StoreAlign);

  Values = std::vector<Value *>{Const64i0, Const32i2};
  auto *Gep2 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(Const64i5, Gep2, StoreAlign);

  Values = std::vector<Value *>{Const64i0, Const32i3};
  auto *Gep3 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(Const32i1, Gep3, StoreAlign);

  // not a tail call
  auto *XorShift1 = IB->CreateCall(xorshift);
  // not exact match
  auto *And1 = IB->CreateAnd(XorShift1, Const64i63);
  Values = std::vector<Value *>{Const64i0, Const32i0};
  auto *Gep4 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(And1, Gep4, StoreAlign);

  auto *XorShift2 = IB->CreateCall(xorshift);
  auto *And2 = IB->CreateAnd(XorShift2, Const64i63);
  Values = std::vector<Value *>{Const64i0, Const32i1};
  auto *Gep5 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(And2, Gep5, StoreAlign);

  auto *XorShift3 = IB->CreateCall(xorshift);
  auto *Trunc1 = IB->CreateTrunc(XorShift3, IB->getInt8Ty());
  Values = std::vector<Value *>{Const64i0, Const32i5, Const32i0};
  auto *Gep6 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(Trunc1, Gep6, StoreAlign);

  auto *XorShift4 = IB->CreateCall(xorshift);
  auto *Trunc2 = IB->CreateTrunc(XorShift4, IB->getInt8Ty());
  Values = std::vector<Value *>{Const64i0, Const32i5, Const32i1};
  auto *Gep7 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(Trunc2, Gep7, StoreAlign);

  auto *XorShift5 = IB->CreateCall(xorshift);
  auto *Trunc3 = IB->CreateTrunc(XorShift5, IB->getInt8Ty());
  Values = std::vector<Value *>{Const64i0, Const32i5, Const32i2};
  auto *Gep8 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  IB->CreateAlignedStore(Trunc3, Gep8, StoreAlign);

  IB->CreateRetVoid();
}

void updateDotGen() {
  auto *Entry = BasicBlock::Create(*Ctx, "Entry", updateDot);
  auto *BB1 = BasicBlock::Create(*Ctx, "BB1", updateDot);
  auto *BB2 = BasicBlock::Create(*Ctx, "BB2", updateDot);
  auto *BB3 = BasicBlock::Create(*Ctx, "BB3", updateDot);
  auto *BB4 = BasicBlock::Create(*Ctx, "BB4", updateDot);
  auto *BB5 = BasicBlock::Create(*Ctx, "BB5", updateDot);
  auto *BBRet = BasicBlock::Create(*Ctx, "BBRet", updateDot);
  IB->SetInsertPoint(Entry);

  auto *DotStruct = updateDot->getArg(0);

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
  IB->CreateCall(changePosition, Args);

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
  Phi->addIncoming(Load1, BB3);

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
  auto *Entry = BasicBlock::Create(*Ctx, "Entry", changeState);
  auto *BB1 = BasicBlock::Create(*Ctx, "BB1", changeState);
  auto *BB2 = BasicBlock::Create(*Ctx, "BB2", changeState);
  IB->SetInsertPoint(Entry);

  auto *DotStruct = changeState->getArg(0);

  IB->CreateBr(BB2);
  IB->SetInsertPoint(BB1);

  IB->CreateRetVoid();
  IB->SetInsertPoint(BB2);

  auto *Phi = IB->CreatePHI(IB->getInt64Ty(), 2);

  auto Values = std::vector<Value *>{Const64i0};
  auto *Gep = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  auto Args = std::vector<Value *>{Gep};
  IB->CreateCall(updateDot, Args);

  // nuw - no unsigned wrap
  // nsw - no signed wrap
  auto *Add = IB->CreateAdd(Phi, Const64i1);
  auto *Icmp = IB->CreateICmpEQ(Add, Const64i10);

  IB->CreateCondBr(Icmp, BB1, BB2);

  // нужно чтобы разнести определение и реализацию
  Phi->addIncoming(Const64i0, Entry);
  Phi->addIncoming(Add, BB2);
}

void distanceGen() {
  auto *Entry = BasicBlock::Create(*Ctx, "Entry", distance);
  IB->SetInsertPoint(Entry);

  auto *Arg0 = distance->getArg(0);
  auto *Arg1 = distance->getArg(1);
  auto *Arg2 = distance->getArg(2);
  auto *Arg3 = distance->getArg(3);

  auto *Icmp1 = IB->CreateICmpUGT(Arg2, Arg0);
  auto *Sub1 = IB->CreateSub(Arg2, Arg0);
  auto *Sub2 = IB->CreateSub(Arg0, Arg2);
  auto Select1 = IB->CreateSelect(Icmp1, Sub1, Sub2);

  auto *Icmp2 = IB->CreateICmpUGT(Arg3, Arg1);
  auto *Sub3 = IB->CreateSub(Arg3, Arg1);
  auto *Sub4 = IB->CreateSub(Arg1, Arg3);
  auto Select2 = IB->CreateSelect(Icmp2, Sub3, Sub3);

  auto *Mul1 = IB->CreateMul(Select1, Select1);
  auto *Mul2 = IB->CreateMul(Select2, Select2);
  auto *Add = IB->CreateAdd(Mul2, Mul1);

  IB->CreateRet(Add);
}

void getNearestDotGen() {
  auto *Entry = BasicBlock::Create(*Ctx, "Entry", getNearestDot);
  auto *BB1 = BasicBlock::Create(*Ctx, "BB1", getNearestDot);
  auto *BB2 = BasicBlock::Create(*Ctx, "BB2", getNearestDot);
  IB->SetInsertPoint(Entry);

  auto *Arg0 = getNearestDot->getArg(0);
  auto *Arg1 = getNearestDot->getArg(1);
  auto *DotStruct = getNearestDot->getArg(2);  

  IB->CreateBr(BB2);
  IB->SetInsertPoint(BB2);
  
  auto *Phi1 = IB->CreatePHI(DotPtr, 2);
  auto *Phi2 = IB->CreatePHI(IB->getInt64Ty(), 2);
  auto *Phi3 = IB->CreatePHI(IB->getInt64Ty(), 2);

  auto Values = std::vector<Value *>{Phi3};
  auto *Gep1 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  
  Values = std::vector<Value *>{Const64i0, Const32i0};
  auto *Gep2 = IB->CreateInBoundsGEP(Dot_t, Gep1, Values);
  auto *Load1 = IB->CreateAlignedLoad(IB->getInt64Ty(), Gep2, LoadAlign);

  Values = std::vector<Value *>{Phi3, Const32i1};
  auto *Gep3 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  auto *Load2 = IB->CreateAlignedLoad(IB->getInt64Ty(), Gep3, LoadAlign);

  auto Args = std::vector<Value *>{Arg0, Arg1, Load1, Load2};
  auto *Call = IB->CreateCall(distance, Args);

  Values = std::vector<Value *>{Phi3, Const32i1};
  auto *Gep4 = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);
  auto *Load3 = IB->CreateAlignedLoad(IB->getInt64Ty(), Gep4, LoadAlign);

  auto *Mul = IB->CreateMul(Load3, Load3);
  auto *Icmp1 = IB->CreateICmpULT(Call, Mul);
  auto *Icmp2 = IB->CreateICmpULT(Call, Phi2);
  auto *And = IB->CreateAnd(Icmp1, Icmp1);
  auto *Select1 = IB->CreateSelect(And, Call, Phi2);
  auto *Select2 = IB->CreateSelect(And, Gep1, Phi1);
  auto *Add = IB->CreateAdd(Phi3, Const64i1);
  auto *Icmp = IB->CreateICmpEQ(Add, Const64i10);

  IB->CreateCondBr(Icmp, BB1, BB2);

  IB->SetInsertPoint(BB1);

  IB->CreateRet(Select2);

  Phi1->addIncoming(DotNullptr, Entry);
  Phi1->addIncoming(Select2, BB2);

  Phi2->addIncoming(Const64i48830, Entry);
  Phi2->addIncoming(Select1, BB2);

  Phi3->addIncoming(Const64i0, Entry);
  Phi3->addIncoming(Add, BB2);
} 

void drawFrameGen() {
  auto *Entry = BasicBlock::Create(*Ctx, "Entry", drawFrame);
  auto *BB1 = BasicBlock::Create(*Ctx, "BB1", drawFrame);
  auto *BB2 = BasicBlock::Create(*Ctx, "BB2", drawFrame);
  auto *BB3 = BasicBlock::Create(*Ctx, "BB3", drawFrame);
  auto *BB4 = BasicBlock::Create(*Ctx, "BB4", drawFrame);
  auto *BB5 = BasicBlock::Create(*Ctx, "BB5", drawFrame);
  auto *BB6 = BasicBlock::Create(*Ctx, "BB6", drawFrame);
  IB->SetInsertPoint(Entry);

  auto *DotStruct = drawFrame->getArg(0);

  IB->CreateBr(BB1);

  IB->SetInsertPoint(BB1);

  auto *Phi1 = IB->CreatePHI(IB->getInt64Ty(), 2);
  auto *Trunc1 = IB->CreateTrunc(Phi1, IB->getInt32Ty());

  IB->CreateBr(BB4);

  IB->SetInsertPoint(BB2);

  IB->CreateRetVoid();

  IB->SetInsertPoint(BB3);

  auto *Add1 = IB->CreateAdd(Phi1, Const64i1);
  auto *Icmp1 = IB->CreateICmpEQ(Add1, Const64i64);

  IB->CreateCondBr(Icmp1, BB2, BB1);

  IB->SetInsertPoint(BB4);

  auto *Phi2= IB->CreatePHI(IB->getInt64Ty(), 2);
  
  auto Args = std::vector<Value *>{Phi1, Phi2, DotStruct};
  auto *Call1 = IB->CreateCall(getNearestDot, Args);

  auto *Icmp2 = IB->CreateICmpEQ(Call1, DotNullptr);
  auto *Trunc2 = IB->CreateTrunc(Phi2, IB->getInt32Ty());

  IB->CreateCondBr(Icmp2, BB6, BB5);

  IB->SetInsertPoint(BB5);

  auto Values = std::vector<Value *>{Const64i0, Const32i5};
  auto *Gep = IB->CreateInBoundsGEP(Dot_t, Call1, Values);
  auto *Bitcast = IB->CreateBitCast(Gep, Int24Ptr);
  auto *Load = IB->CreateAlignedLoad(Int24_t, Bitcast, LoadAlign);

  IB->CreateBr(BB6);

  IB->SetInsertPoint(BB6);

  auto *Phi3= IB->CreatePHI(Int24_t, 2);

  Args = std::vector<Value *>{Trunc1, Trunc2, Phi3};
  auto *Call2 = IB->CreateCall(simPutPixel, Args); 

  auto *Add2 = IB->CreateAdd(Phi2, Const64i1);
  auto *Icmp3 = IB->CreateICmpEQ(Add2, Const64i64);

  IB->CreateCondBr(Icmp3, BB3, BB4);

  Phi1->addIncoming(Const64i0, Entry);
  Phi1->addIncoming(Add1, BB3);

  Phi2->addIncoming(Const64i0, BB1);
  Phi2->addIncoming(Add2, BB6);

  Phi3->addIncoming(Load, BB5);
  Phi3->addIncoming(Const24i657930, BB4);
}

void initDotsGen() {
  auto *Entry = BasicBlock::Create(*Ctx, "Entry", initDots);
  auto *BB1 = BasicBlock::Create(*Ctx, "BB1", initDots);
  auto *BB2 = BasicBlock::Create(*Ctx, "BB2", initDots);
  IB->SetInsertPoint(Entry);

  auto *DotStruct = initDots->getArg(0);

  IB->CreateBr(BB2);

  IB->SetInsertPoint(BB1);
  
  IB->CreateRetVoid();

  IB->SetInsertPoint(BB2);

  auto *Phi = IB->CreatePHI(IB->getInt64Ty(), 2);

  auto Values = std::vector<Value *>{Phi};
  auto *Gep = IB->CreateInBoundsGEP(Dot_t, DotStruct, Values);

  auto Args = std::vector<Value *>{Gep};
  IB->CreateCall(changePosition, Args);

  auto *Add = IB->CreateAdd(Phi, Const64i1);
  auto *Icmp = IB->CreateICmpEQ(Add, Const64i10);

  IB->CreateCondBr(Icmp, BB1, BB2);

  Phi->addIncoming(Const64i0, Entry);
  Phi->addIncoming(Add, BB2);
}

// Генерация для одного фрейма
void appGen() {
  auto *Entry = BasicBlock::Create(*Ctx, "Entry", app);
  IB->SetInsertPoint(Entry);

  auto *DotsArray_t = ArrayType::get(Dot_t, 10);

  auto *Alloca = IB->CreateAlloca(DotsArray_t);
  auto *Bitcast = IB->CreateBitCast(Alloca, IB->getInt8PtrTy());

  auto Values = std::vector<Value *>{Const64i0, Const64i0};
  auto *Gep = IB->CreateInBoundsGEP(DotsArray_t, Alloca, Values);

  auto Args = std::vector<Value *>{Gep};
  IB->CreateCall(initDots, Args);
  IB->CreateCall(changeState, Args);
  IB->CreateCall(drawFrame, Args);

  Args = std::vector<Value *>{};
  IB->CreateCall(simFlush, Args);

  IB->CreateRetVoid();
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

void createFunctions() {
  assert(Dot_t && RGB_t);
  auto *xorshift_t = FunctionType::get(IB->getInt64Ty(), /*isVarArg*/ false);
  xorshift = 
    Function::Create(xorshift_t, Function::ExternalLinkage, "xorshift", *M);

  auto *changePosition_t = FunctionType::get(IB->getVoidTy(), 
                                             PointerType::getUnqual(Dot_t), 
                                             /*isVarArg*/ false);
  changePosition =
    Function::Create(changePosition_t, Function::ExternalLinkage, "changePosition", *M);

  auto *updateDot_t = FunctionType::get(IB->getVoidTy(), 
                                        PointerType::getUnqual(Dot_t), 
                                        /*isVarArg*/ false);
  updateDot =
    Function::Create(updateDot_t, Function::ExternalLinkage, "updateDot", *M);

  auto *changeState_t = FunctionType::get(IB->getVoidTy(), 
                                          PointerType::getUnqual(Dot_t), 
                                          /*isVarArg*/ false);
  changeState =
    Function::Create(changeState_t, Function::ExternalLinkage, "changeState", *M);

  auto Args = std::vector<Type *>{IB->getInt64Ty(), IB->getInt64Ty(), 
                                  IB->getInt64Ty(), IB->getInt64Ty()};
  auto *distance_t = FunctionType::get(IB->getInt64Ty(), 
                                       Args, 
                                       /*isVarArg*/ false);
  distance =
    Function::Create(distance_t, Function::ExternalLinkage, "distance", *M);

  // non const ptr
  Args = std::vector<Type *>{IB->getInt64Ty(), IB->getInt64Ty(), 
                             PointerType::getUnqual(Dot_t)};
  auto *getNearestDot_t = FunctionType::get(PointerType::getUnqual(Dot_t), 
                                            Args, 
                                            /*isVarArg*/ false);
  getNearestDot = 
    Function::Create(getNearestDot_t, Function::ExternalLinkage, "getNearestDot", *M);

  auto *drawFrame_t = FunctionType::get(IB->getVoidTy(), 
                                        PointerType::getUnqual(Dot_t), 
                                        /*isVarArg*/ false);
  drawFrame =
    Function::Create(drawFrame_t, Function::ExternalLinkage, "drawFrame", *M);

  Args = std::vector<Type *>{IB->getInt32Ty(), IB->getInt32Ty(), Int24_t};
  auto simPutPixel_t = FunctionType::get(IB->getVoidTy(), 
                                         Args, 
                                         /*isVarArg*/ false);
  simPutPixel = 
    M->getOrInsertFunction("simPutPixel", simPutPixel_t);

  auto simFlush_t = FunctionType::get(IB->getVoidTy(), 
                                      IB->getVoidTy(), 
                                      /*isVarArg*/ false);
  simFlush = 
    M->getOrInsertFunction("simFlush", simFlush_t);

  auto *initDots_t = FunctionType::get(IB->getVoidTy(), 
                                        PointerType::getUnqual(Dot_t), 
                                        /*isVarArg*/ false);
  initDots = 
    Function::Create(initDots_t, Function::ExternalLinkage, "initDots", *M);

  auto *app_t = FunctionType::get(IB->getVoidTy(), 
                                  IB->getVoidTy(), 
                                  /*isVarArg*/ false);
  app = 
    Function::Create(app_t, Function::ExternalLinkage, "app", *M);
}

void createConstants() {
  Int24_t = IB->getIntNTy(24);
  Int24Ptr = PointerType::getUnqual(Int24_t); 
  DotPtr = PointerType::getUnqual(Dot_t);
  DotNullptr = ConstantPointerNull::get(DotPtr);
  Const32ineg1 = ConstantInt::get(IB->getInt32Ty(), -1);
  Const64i0 = ConstantInt::get(IB->getInt64Ty(), 0);
  Const32i0 = ConstantInt::get(IB->getInt32Ty(), 0);
  Const64i1 = ConstantInt::get(IB->getInt64Ty(), 1);
  Const32i1 = ConstantInt::get(IB->getInt32Ty(), 1);
  Const32i2 = ConstantInt::get(IB->getInt32Ty(), 2);
  Const32i3 = ConstantInt::get(IB->getInt32Ty(), 3);
  Const32i4 = ConstantInt::get(IB->getInt32Ty(), 4);
  Const64i5 = ConstantInt::get(IB->getInt64Ty(), 5);
  Const32i5 = ConstantInt::get(IB->getInt32Ty(), 5);
  Const64i10 = ConstantInt::get(IB->getInt64Ty(), 10);
  Const64i63 = ConstantInt::get(IB->getInt64Ty(), 63);
  Const64i64 = ConstantInt::get(IB->getInt64Ty(), 64);
  Const64i48830 = ConstantInt::get(IB->getInt64Ty(), 48830);
  Const24i657930 = ConstantInt::get(Int24_t, 657930);
}

int main() {
  Ctx = std::make_unique<LLVMContext>();
  M = std::make_unique<Module>("Handmade-IR", *Ctx);
  IB = std::make_unique<IRBuilder<>>(*Ctx);
  createStructs();
  createConstants();
  createFunctions();
  assert(RGB_t && Dot_t);

  xorShiftGen();
  changePositionGen();
  updateDotGen();
  changeStateGen();
  distanceGen();
  getNearestDotGen();
  drawFrameGen();
  initDotsGen();
  appGen();
  
  M->print(outs(), nullptr);

  outs() << "\n\n==ModuleVerifier==\n";
  assert(verifyModule(*M, &outs()));

  outs() << "\n\n==Execution==\n";
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();

  auto *EE = EngineBuilder(std::move(M)).create();
  auto FuncCreator = [&](const std::string &FuncName) {
    if (FuncName == "simFlush")
      return reinterpret_cast<void *>(lib::simFlush);
    if (FuncName == "simPutPixel")
      return reinterpret_cast<void *>(lib::simPutPixel);
    llvm_unreachable("Unknown function");
  };
  EE->InstallLazyFunctionCreator(FuncCreator);
  EE->finalizeObject();
  auto Args = std::vector<GenericValue>{};
  
  lib::simInit();
  EE->runFunction(app, Args);
  lib::simExit();

  outs() << "==End==\n";
}