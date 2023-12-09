#include "../include/Translator.h"
#include "../include/Assembler.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <unordered_map>
#include <utility>

#define MAKE_INSTR(name)                                     \
  if (GlobalIt == End ||                                     \
      !std::holds_alternative<Token::Word>(GlobalIt->Value)) \
      return false;                                          \
    auto Name = GlobalIt->getName();                         \
    if (Name != name)                                        \
      return false;                                          \
    auto Instr = Instruction{}.addOpcode(Name);              \
    GlobalIt++;                                              \
    DEBUG_EXPR(std::cout << "parsed {" << name << "}"        \
               << std::endl);

namespace translator {

namespace {

namespace tokenizer {

void removeComments(std::string &Program) {
  constexpr auto Comment = '#';
  constexpr auto CommentSubstitution = ' ';
  auto CommentBeg = std::find(Program.begin(), Program.end(), Comment);
  while (CommentBeg != Program.end()) {
    auto CommentEnd = std::find(std::next(CommentBeg), Program.end(), Comment);
    if (CommentEnd == Program.end())
      utils::reportFatalError("Wrong comments format");
    std::fill(CommentBeg, CommentEnd + 1, CommentSubstitution);
    CommentBeg = std::find(CommentEnd, Program.end(), Comment);
  }
}

template <typename T>
std::optional<Token> tryTokenize(const std::string &Word) {
  if (T::isa(Word))
    return Token{T{Word}};
  return std::nullopt;
}

Token getToken(std::string Word) {
  auto Res = std::optional<Token>{};
  if (Res = tryTokenize<Token::Section>(Word))
    return *Res;
  if (Res = tryTokenize<Token::Function>(Word))
    return *Res;
  if (Res = tryTokenize<Token::Label>(Word))
    return *Res;
  if (Res = tryTokenize<Token::Assign>(Word))
    return *Res;
  if (Res = tryTokenize<Token::Word>(Word))
    return *Res;
  utils::reportFatalError("Unreachable");
  return *Res;
}

} // namespace tokenizer

namespace parser {

using namespace assembler;
using TokenIt = std::vector<Token>::iterator;

class Parser {
  TokenIt GlobalIt;
  TokenIt End;

  void checkIterator() {
    if (GlobalIt == End)
      utils::reportFatalError("Incomplete lexem");
  }

  bool parseConstant(GlobalConfig &Cfg) {
    if (GlobalIt == End || 
        !std::holds_alternative<Token::Word>(GlobalIt->Value))
      return false;
    auto Name = GlobalIt->getName();
    if (Name == "start") 
      return false;

    GlobalIt++;
    auto Val = utils::stoll(GlobalIt->getName());
    checkIterator();
    GlobalIt++;
    DEBUG_EXPR(std::cout << "parsed constant" + GlobalIt->getName() << std::endl);
    Cfg.addGlobal(Global{}.addName(Name).addInitVal(Val));
    return true;
  }

  bool parseStart(GlobalConfig &Cfg) {
    if (GlobalIt == End || 
        !std::holds_alternative<Token::Word>(GlobalIt->Value))
      return false;
    auto Name = GlobalIt->getName();
    if (Name != "start") 
      return false;

    if (Cfg.hasStart())
      utils::reportFatalError("Start function multi definition");

    GlobalIt++;
    Cfg.addStart(GlobalIt->getName());
    DEBUG_EXPR(std::cout << "parsed start " + GlobalIt->getName() << std::endl);
    checkIterator();
    GlobalIt++;
    return true;
  }

  bool parseGlobals(Code &Code) {
    if (!std::holds_alternative<Token::Section>(GlobalIt->Value))
      utils::reportFatalError("Can't parse globals section: " + 
                              GlobalIt->getName());
    if (GlobalIt->getName() != "global")
      return false;
    
    GlobalIt++;
    auto HadProgress = false;
    auto Parsed = false;
    auto Cfg = GlobalConfig{};

    do {
      Parsed = false;
      Parsed |= parseConstant(Cfg);
      Parsed |= parseStart(Cfg);
      HadProgress |= Parsed;
    } while (Parsed);

    Code.addGlobalConf(std::move(Cfg));
    DEBUG_EXPR(std::cout << "parsed globals" << std::endl);
    return HadProgress;
  }

  void parseImm(Instruction &I) {
    checkIterator();
    I.addArgument(utils::stoll(GlobalIt->getName()));
    GlobalIt++;
  }

  void parseReg(Instruction &I) {
    checkIterator();
    I.addArgument(Register{GlobalIt->getName()});
    GlobalIt++;
  }

  void parseRetVal(Instruction &I) {
    checkIterator();
    if (!std::holds_alternative<Token::Assign>(GlobalIt->Value))
      utils::reportFatalError("Wrong ret format");
    GlobalIt++;
    checkIterator();
    I.addReturnValue(Register{GlobalIt->getName()});
    GlobalIt++;
  }

  void parseLabel(Instruction &I) {
    checkIterator();
    I.addArgument(Label{GlobalIt->getName()});
    GlobalIt++;
  }

  bool parseAnd(BasicBlock &BB) {
    MAKE_INSTR("and");
    parseReg(Instr);
    parseImm(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseRet(BasicBlock &BB) {
    MAKE_INSTR("ret");
    BB.addInstruction(Instr);
    return true;
  }

  bool parseCmpEqImm(BasicBlock &BB) {
    MAKE_INSTR("cmpEqImm");
    parseReg(Instr);
    parseImm(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseBrCond(BasicBlock &BB) {
    MAKE_INSTR("brCond");
    parseReg(Instr);
    parseLabel(Instr);
    parseLabel(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseBr(BasicBlock &BB) {
    MAKE_INSTR("br");
    parseLabel(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseCall(BasicBlock &BB) {
    MAKE_INSTR("call");
    parseLabel(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseCmpUGTImm(BasicBlock &BB) {
    MAKE_INSTR("cmpUGTImm");
    parseReg(Instr);
    parseImm(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseCmpUGT(BasicBlock &BB) {
    MAKE_INSTR("cmpUGT");
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseLi(BasicBlock &BB) {
    MAKE_INSTR("li");
    parseImm(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseMul(BasicBlock &BB) {
    MAKE_INSTR("mul");
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseAdd(BasicBlock &BB) {
    MAKE_INSTR("add");
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseSub(BasicBlock &BB) {
    MAKE_INSTR("sub");
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseSelect(BasicBlock &BB) {
    MAKE_INSTR("select");
    parseReg(Instr);
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseMv(BasicBlock &BB) {
    MAKE_INSTR("mv");
    parseReg(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseNormalInstruction(BasicBlock &BB) {
    auto Parsed = false;

    Parsed |= parseAnd(BB);
    Parsed |= parseRet(BB);
    Parsed |= parseCmpEqImm(BB);
    Parsed |= parseBrCond(BB);
    Parsed |= parseBr(BB);
    Parsed |= parseCall(BB);
    Parsed |= parseCmpUGTImm(BB);
    Parsed |= parseCmpUGT(BB);
    Parsed |= parseLi(BB);
    Parsed |= parseMul(BB);
    Parsed |= parseAdd(BB);
    Parsed |= parseSub(BB);
    Parsed |= parseSelect(BB);
    Parsed |= parseMv(BB);

    return Parsed;
  }

  bool parseIncJump(BasicBlock &BB) {
    MAKE_INSTR("incJump");
    parseReg(Instr);
    parseImm(Instr);
    parseLabel(Instr);
    parseLabel(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  } 

  bool parseLoadDotField(BasicBlock &BB) {
    MAKE_INSTR("loadDotField");
    parseReg(Instr);
    parseImm(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseStoreDotField(BasicBlock &BB) {
    MAKE_INSTR("storeDotField");
    parseReg(Instr);
    parseImm(Instr);
    parseReg(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseStoreDotFieldImm(BasicBlock &BB) {
    MAKE_INSTR("storeDotFieldImm");
    parseReg(Instr);
    parseImm(Instr);
    parseImm(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseJumpIfDot(BasicBlock &BB) {
    MAKE_INSTR("jumpIfDot");
    parseReg(Instr);
    parseImm(Instr);
    parseReg(Instr);
    parseLabel(Instr);
    parseLabel(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseGetDotAddr(BasicBlock &BB) {
    MAKE_INSTR("getDotAddr");
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseInitRgb(BasicBlock &BB) {
    MAKE_INSTR("initRgb");
    parseReg(Instr);
    parseLabel(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseLoadRgb(BasicBlock &BB) {
    MAKE_INSTR("loadRgb");
    parseReg(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseCmpTwo(BasicBlock &BB) {
    MAKE_INSTR("cmpTwo");
    parseReg(Instr);
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseNorm(BasicBlock &BB) {
    MAKE_INSTR("norm");
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseXorshift(BasicBlock &BB) {
    MAKE_INSTR("xorshift");
    parseLabel(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseCreateDots(BasicBlock &BB) {
    MAKE_INSTR("createDots");
    parseImm(Instr);
    parseRetVal(Instr);
    BB.addInstruction(Instr);
    return true;
  }

  bool parseSpecialInstruction(BasicBlock &BB) {
    auto Parsed = false;

    Parsed |= parseIncJump(BB);
    Parsed |= parseLoadDotField(BB);
    Parsed |= parseStoreDotField(BB);
    Parsed |= parseStoreDotFieldImm(BB);
    Parsed |= parseJumpIfDot(BB);
    Parsed |= parseGetDotAddr(BB);
    Parsed |= parseInitRgb(BB);
    Parsed |= parseLoadRgb(BB);
    Parsed |= parseCmpTwo(BB);
    Parsed |= parseNorm(BB);
    Parsed |= parseXorshift(BB);
    Parsed |= parseCreateDots(BB);

    return Parsed;
  }

  bool parseBasicBlock(Function &Func) {
    if (GlobalIt == End || 
        !std::holds_alternative<Token::Label>(GlobalIt->Value))
      return false;

    auto HadProgress = false;
    auto Parsed = false;
    auto BB = BasicBlock{}.addLabel(GlobalIt->getName());
    checkIterator();
    GlobalIt++;

    do {
      Parsed = false;
      Parsed |= parseNormalInstruction(BB);
      Parsed |= parseSpecialInstruction(BB);
      HadProgress |= Parsed;
    } while (Parsed);
    Func.addBlock(BB);
    return HadProgress;
  }

  bool parseFunction(Code &Code) {
    if (GlobalIt == End || 
        !std::holds_alternative<Token::Function>(GlobalIt->Value))
      return false;
    
    auto Name = GlobalIt->getName();
    auto Func = Function{}.addName(Name);
    GlobalIt++;
    auto HadProgress = false;
    auto Parsed = false;

    do {
      Parsed = false;
      Parsed |= parseBasicBlock(Func);
      HadProgress |= Parsed;
    } while (Parsed);
    Code.addFunction(Func);
    DEBUG_EXPR(std::cout << "parsed function: " << Name 
              << "\n" << std::endl);
    return HadProgress;
  }

  bool parseText(Code &Code) {
    if (!std::holds_alternative<Token::Section>(GlobalIt->Value))
      utils::reportFatalError("Can't parse text section");
    if (GlobalIt->getName() != "text")
      return false;
    
    GlobalIt++;
    auto HadProgress = false;
    auto Parsed = false;

    do {
      Parsed = false;
      Parsed |= parseFunction(Code);
      HadProgress |= Parsed;
    } while (Parsed);
    DEBUG_EXPR(std::cout << "parsed text\n" << std::endl);
    return HadProgress;
  }

  void parseSections(Code &Code) {
    auto Parsed = false;

    do {
      Parsed = false;
      Parsed |= parseGlobals(Code);
      Parsed |= parseText(Code);
      DEBUG_EXPR(std::cout << "parsed section\n" << std::endl);
    } while (GlobalIt != End && Parsed);
  }

public:
  Code parse(std::vector<Token> &Program) {
    GlobalIt = Program.begin();
    End = Program.end();
    Code Code;
    parseSections(Code);
    if (GlobalIt != End)
      utils::reportFatalError("Parsing stoped on " + GlobalIt->getName());
    return Code;
  }
};  

} // namespace parser

namespace IR {

using namespace assembler;

class Generator {
protected:
  using BBMap_t = std::unordered_map<std::string, llvm::BasicBlock *>;

  struct InstructionEnv {
    BBMap_t &BBMap;
    const std::string &FuncName;
  };

  struct BasicBlockEnv {
    BBMap_t &BBMap;
    const std::string &FuncName;
  };

  // each function has its own register file
  constexpr static auto TmpRegFileSize = 12u;
  // argument registers file is unique
  constexpr static auto ArgsRegFileSize = 3u;

  std::unique_ptr<llvm::Module> M;
  std::unique_ptr<llvm::LLVMContext> Ctx;
  std::unique_ptr<llvm::IRBuilder<>> IB;
  
  llvm::ArrayType *TmpRegFile_t = nullptr;
  llvm::GlobalVariable *TmpRegFile = nullptr; 
  llvm::ArrayType *ArgRegFile_t = nullptr;
  llvm::GlobalVariable *ArgRegFile = nullptr;
  llvm::ArrayType *RetValReg_t = nullptr;
  llvm::GlobalVariable *RetValReg = nullptr;

  // offset in tp reg file in register sizes
  std::unordered_map<std::string, size_t> FuncToRegFileMap;
  std::unordered_map<std::string, llvm::Function*> FuncMap;
  llvm::Function *Start;

private:
  auto *getArgRegFile() {
    ArgRegFile_t = llvm::ArrayType::get(IB->getInt64Ty(), ArgsRegFileSize);
    M->getOrInsertGlobal("argRegFile", ArgRegFile_t);
    return M->getNamedGlobal("argRegFile");
  }

  auto *getTmpRegFile() {
    auto NumOfRegs = FuncToRegFileMap.size() * TmpRegFileSize;
    TmpRegFile_t = llvm::ArrayType::get(IB->getInt64Ty(), NumOfRegs);
    M->getOrInsertGlobal("tmpRegFile", TmpRegFile_t);
    return M->getNamedGlobal("tmpRegFile");
  }

  auto *getRetValReg() {
    RetValReg_t = llvm::ArrayType::get(IB->getInt64Ty(), 1);
    M->getOrInsertGlobal("retValReg", RetValReg_t);
    return M->getNamedGlobal("retValReg");
  }
  
  void generateFunctions(Code &Code) {
    auto GlobalCfg = Code.getGlobal();
    if (!GlobalCfg.hasStart())
      utils::reportFatalError("No start function");
    
    auto StartFuncName = GlobalCfg.getStart();

    for (auto &F : Code.getFunctions()) {
      auto FuncName = F.getName();
      FuncToRegFileMap[FuncName] = FuncToRegFileMap.size() * TmpRegFileSize;
      auto *FuncType = llvm::FunctionType::get(IB->getVoidTy(), /*isVarArg*/ false);
      auto *NewFunc = llvm::Function::Create(FuncType, llvm::Function::ExternalLinkage, 
                                             FuncName, *M);
      assert(NewFunc);
      auto [_, Inserted] = FuncMap.emplace(FuncName, NewFunc);
      if (!Inserted)
        utils::reportFatalError("Redefinition of <" + FuncName + ">");
      if (FuncName == StartFuncName)
        Start = NewFunc;
    }
  } 

  void generateGlobals(Code &Code) {
    ArgRegFile = getArgRegFile();
    TmpRegFile = getTmpRegFile();
    RetValReg = getRetValReg();

    auto Cfg = Code.getGlobal();
    for (auto &G : Cfg.getGlobals()) {
      auto *GlobT = IB->getInt64Ty();
      auto GlobVal = llvm::APInt{/*NumBits*/ 64, 
                                 static_cast<size_t>(G.getInitVal())};
      auto GlobAlign = llvm::MaybeAlign{8};
      auto *GV = 
        new llvm::GlobalVariable(*M, GlobT,
                  /*isConstant*/ false,
                  /*Linkage*/ llvm::GlobalValue::ExternalLinkage,
                  /*Initializer*/ nullptr,
                  G.getName());
      GV->setAlignment(GlobAlign);
      GV->setInitializer(llvm::Constant::getIntegerValue(GlobT, GlobVal));
    }
  }

  void generateInBB(BasicBlock &BB, BasicBlockEnv BBEnv) {
    auto BBName = BB.getLabel();
    auto &BBMap = BBEnv.BBMap;
    assert(BBMap.find(BBName) != BBMap.end());
    auto *LLVMBB = BBMap[BBName];
    assert(LLVMBB);
    IB->SetInsertPoint(LLVMBB);

    for (auto &I : BB.getInstructions())
      generateInstruction(I, InstructionEnv{BBMap, BBEnv.FuncName});
  }

  void generateInFunction(Function &F) {
    auto BBMap = BBMap_t{};
    auto FuncName = F.getName();
    assert(FuncMap.find(FuncName) != FuncMap.end());
    auto *LLVMFunc = FuncMap[FuncName];

    for (auto &BB : F.getBlocks()) {
      auto BBName = BB.getLabel();
      auto *LLVMBB = llvm::BasicBlock::Create(*Ctx, BBName, LLVMFunc);
      assert(LLVMBB);
      auto [_, Inserted] = BBMap.emplace(BBName, LLVMBB);
      if (!Inserted)
        utils::reportFatalError("Duplicating basic block: " + BBName + 
                                " in <" + FuncName + ">");
    }

    for (auto &BB : F.getBlocks())
      generateInBB(BB, BasicBlockEnv{BBMap, F.getName()});
  }

protected:
  virtual void generateInstruction(Instruction &I, InstructionEnv Env) = 0;
  virtual void initAllTypes() = 0;

public: 
  virtual ~Generator() = default;

  IRToExecute::IR_t generateIR(Code &Code) {
    Ctx = std::make_unique<llvm::LLVMContext>();
    M = std::make_unique<llvm::Module>("Pseudo-IR", *Ctx);
    IB = std::make_unique<llvm::IRBuilder<>>(*Ctx);
    initAllTypes();

    generateFunctions(Code);
    generateGlobals(Code);
    
    for (auto &F : Code.getFunctions())
      generateInFunction(F);
    return {std::move(M), std::move(Ctx)};
  }

  llvm::Function *getStartFunc() { return Start; }

  virtual 
  std::pair<IRToExecute::Mapping_t, IRToExecute::RegFile_t> generateMapper() = 0;
};


class ControlFlowGenerator : public Generator {  
protected:
  llvm::StructType *Dot_t = nullptr;
  llvm::StructType *RGB_t = nullptr;

private:
  void generateRet(Instruction &I, InstructionEnv &Env) {
    IB->CreateRetVoid();
  }

  void generateIncJump(Instruction &I, InstructionEnv &Env) {
    auto *Const1i64 = llvm::ConstantInt::get(IB->getInt64Ty(), 1);

    auto Reg = std::get<Register>(I.getArg(0));
    auto *RegToInc = generateRegVal(Reg, Env.FuncName);
    auto ImmToCmpVal = std::get<Immidiate>(I.getArg(1));
    auto *ImmToCmp = llvm::ConstantInt::get(IB->getInt64Ty(), ImmToCmpVal);
    auto Label1Name = std::get<Label>(I.getArg(2));
    assert(Env.BBMap.find(Label1Name) != Env.BBMap.end());
    auto *Label1 = Env.BBMap[Label1Name];
    auto Label2Name = std::get<Label>(I.getArg(3));
    assert(Env.BBMap.find(Label2Name) != Env.BBMap.end());
    auto *Label2 = Env.BBMap[Label2Name];

    auto *Val = IB->CreateAdd(RegToInc, Const1i64);
    auto *Cond = IB->CreateICmpEQ(Val, ImmToCmp);

    IB->CreateCondBr(Cond, Label1, Label2);
  }

  void generateJumpIfDot(Instruction &I, InstructionEnv &Env) {
    auto *Const0i64 = llvm::ConstantInt::get(IB->getInt64Ty(), 0); 

    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtrI64 = generateRegVal(StructPtrReg, Env.FuncName);
    auto ImmOffVal = std::get<Immidiate>(I.getArg(1));
    auto *ImmOff = llvm::ConstantInt::get(IB->getInt32Ty(), ImmOffVal);
    auto RegToCmp = std::get<Register>(I.getArg(2));
    auto *ValCmpWith = generateRegVal(RegToCmp, Env.FuncName);
    auto Label1Name = std::get<Label>(I.getArg(3));
    assert(Env.BBMap.find(Label1Name) != Env.BBMap.end());
    auto *Label1 = Env.BBMap[Label1Name];
    auto Label2Name = std::get<Label>(I.getArg(4));
    assert(Env.BBMap.find(Label2Name) != Env.BBMap.end());
    auto *Label2 = Env.BBMap[Label2Name];
    
    auto *StructPtr = IB->CreateBitCast(StructPtrI64, llvm::PointerType::getUnqual(Dot_t));
    auto Values = std::vector<llvm::Value *>{Const0i64, ImmOff};
    auto *Ptr = IB->CreateInBoundsGEP(Dot_t, StructPtr, Values);
    auto *Field = IB->CreateAlignedLoad(IB->getInt64Ty(), Ptr, llvm::MaybeAlign{8});
    auto *Cond = IB->CreateICmpUGT(ValCmpWith, Field);

    IB->CreateCondBr(Cond, Label1, Label2);
  }

  void generateBrCond(Instruction &I, InstructionEnv &Env) {
    
    auto CondReg = std::get<Register>(I.getArg(0));
    auto *Cond = generateRegVal(CondReg, Env.FuncName);
    auto Label1Name = std::get<Label>(I.getArg(1));
    assert(Env.BBMap.find(Label1Name) != Env.BBMap.end());
    auto *Label1 = Env.BBMap[Label1Name];
    auto Label2Name = std::get<Label>(I.getArg(2));
    assert(Env.BBMap.find(Label2Name) != Env.BBMap.end());
    auto *Label2 = Env.BBMap[Label2Name];

    IB->CreateCondBr(Cond, Label1, Label2);
  }

  void generateBr(Instruction &I, InstructionEnv &Env) {
    auto LabelName = std::get<Label>(I.getArg(0));
    assert(Env.BBMap.find(LabelName) != Env.BBMap.end());
    auto *Label = Env.BBMap[LabelName];

    IB->CreateBr(Label);
  }

  void generateExternalCall(const std::string &Name) {
    auto Args = std::vector<llvm::Type *>{};
    auto Func_t = llvm::FunctionType::get(IB->getVoidTy(), 
                                          Args, 
                                          /*isVarArg*/ false);
    auto FuncCallee = M->getOrInsertFunction(Name, Func_t);

    IB->CreateCall(FuncCallee);
  }

  void generateCall(Instruction &I, InstructionEnv &Env) {
    auto FuncName = std::get<Label>(I.getArg(0));
    if (FuncMap.find(FuncName) == FuncMap.end()) {
      generateExternalCall(FuncName);
      return;
    }
    auto FuncPtr = FuncMap[FuncName];

    IB->CreateCall(FuncPtr);
  }

  void initAllTypes() override {
    auto RGBElements = std::vector<llvm::Type *>{IB->getInt8Ty(),
                                         IB->getInt8Ty(),
                                         IB->getInt8Ty()};
    RGB_t = llvm::StructType::create(*Ctx, RGBElements, "RGB");

    auto DotElements = std::vector<llvm::Type *>{IB->getInt64Ty(),
                                         IB->getInt64Ty(),
                                         IB->getInt64Ty(),
                                         IB->getInt32Ty(),
                                         IB->getInt64Ty(),
                                         RGB_t};
    Dot_t = llvm::StructType::create(*Ctx, DotElements, "Dot");
  }

  bool generateControlFlowInstruction(Instruction &I, InstructionEnv Env) {
    auto Name = I.getOpcode();
    if (Name == "ret") {
      generateRet(I, Env);
      return true;
    }
    if (Name == "incJump") {
      generateIncJump(I, Env);
      return true;
    }
    if (Name == "jumpIfDot") {
      generateJumpIfDot(I, Env);
      return true;
    }
    if (Name == "brCond") {
      generateBrCond(I, Env);
      return true;
    }
    if (Name == "br") {
      generateBr(I, Env);
      return true;
    }
    if (Name == "call") {
      generateCall(I, Env);
      return true;
    }
    return false;
  }

protected:
  llvm::Value *generateRegVal(Register &Reg, const std::string &FuncName) {
    if (Reg.getClass() == 'r') {
      auto *Gep = IB->CreateConstGEP2_64(RetValReg_t, RetValReg, 0, Reg.getNumber());
      return IB->CreateLoad(IB->getInt64Ty(), Gep);
    }
    if (Reg.getClass() == 'a') {
      auto *Gep = IB->CreateConstGEP2_64(ArgRegFile_t, ArgRegFile, 0, Reg.getNumber());
      return IB->CreateLoad(IB->getInt64Ty(), Gep);
    }
    if (Reg.getClass() == 't') {
      assert(FuncToRegFileMap.find(FuncName) != FuncToRegFileMap.end());
      auto RegFileOffset = FuncToRegFileMap[FuncName];
      auto *Gep = IB->CreateConstGEP2_64(TmpRegFile_t, TmpRegFile, 0, 
                                         RegFileOffset + Reg.getNumber());
      return IB->CreateLoad(IB->getInt64Ty(), Gep);
    }
    utils::reportFatalError("Unknown reg class");
    return nullptr;
  }

  virtual void generateDataFlowInstruction(Instruction &I, InstructionEnv Env) = 0;

public:
  virtual ~ControlFlowGenerator() = default;

  void generateInstruction(Instruction &I, InstructionEnv Env) override {
    if (generateControlFlowInstruction(I, Env))
      return;
    generateDataFlowInstruction(I, Env);
  } 
  
  std::pair<IRToExecute::Mapping_t, IRToExecute::RegFile_t> generateMapper() override {
    auto RegFilePtr = std::unique_ptr<uint64_t>{};
    auto *BufPtr = RegFilePtr.get();
    auto Mapper = [BufPtr](const std::string &FuncName) -> void * { 
        return nullptr; 
      };
    return {std::move(Mapper), std::move(RegFilePtr)};
  }
};

class PseudoGenerator final : public ControlFlowGenerator {

  void generateDataFlowInstruction(Instruction &I, InstructionEnv Env) override {

    //utils::reportFatalError("Unknown instruction:");
  }

public:
  std::pair<IRToExecute::Mapping_t, IRToExecute::RegFile_t> generateMapper() override {
    auto RegFilePtr = std::unique_ptr<uint64_t>{};
    auto *BufPtr = RegFilePtr.get();
    auto Mapper = [BufPtr](const std::string &FuncName) -> void * { 
        return nullptr; 
      };
    return {std::move(Mapper), std::move(RegFilePtr)};
  }
};

} // namespace IR

} // namespace

std::vector<Token> tokenize(std::string Program) {
  tokenizer::removeComments(Program);
  std::replace(Program.begin(), Program.end(), ',', ' ');

  constexpr auto Delimiters = std::array{' ', '\n', '\t'};
  auto NotDelim = [Delimiters] (char C) { 
    return std::find(Delimiters.begin(), Delimiters.end(), C) == Delimiters.end(); 
  };
  auto WordBeg = std::find_if(Program.begin(), Program.end(), NotDelim);
  
  auto Res = std::vector<Token>{};
  while (WordBeg != Program.end()) {
    auto WordEnd = std::find_if_not(WordBeg, Program.end(), NotDelim);
    if (WordBeg != WordEnd)
      Res.emplace_back(tokenizer::getToken({WordBeg, WordEnd}));
    WordBeg = std::find_if(WordEnd, Program.end(), NotDelim);
  }

  return Res;
}

assembler::Code parse(std::vector<Token> Program) {
  return parser::Parser{}.parse(Program);
}

IRToExecute makePseudoLLVMIR(assembler::Code &Code) {
  auto Gen = IR::PseudoGenerator{};
  auto [Mapper, RegFile] = Gen.generateMapper();
  return {Gen.generateIR(Code), std::move(Mapper), 
          std::move(RegFile), Gen.getStartFunc()};
}

} // namespace translator