#include "Translator.h"
#include "Assembler.h"
#include "Graphics.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <unordered_map>
#include <utility>
#include <array>

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

struct Dot {
  uint64_t X;
  uint64_t Y;
  uint64_t GrowthSpeed;
  int64_t GrowthDirection;
  uint64_t R;
  struct lib::RGB Colour;
};

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
  constexpr static auto ArgsRegFileSize = 4u;

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
  std::unordered_map<std::string, llvm::GlobalVariable*> GVMap;
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
      GVMap[G.getName()] = GV;
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
  std::pair<IRToExecute::Mapping_t, IRToExecute::RegisterState> generateMapper() = 0;
};

class ControlFlowGenerator : public Generator {  
protected:
  llvm::StructType *Dot_t = nullptr;
  llvm::StructType *RGB_t = nullptr;

  llvm::Value *generateRegValueLoad(Register &Reg, const std::string &FuncName) {
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

  void generateStoreToReg(Register Reg, llvm::Value *Val, const std::string &FuncName) {
    if (Reg.getClass() == 'r') {
      auto *Gep = IB->CreateConstGEP2_64(RetValReg_t, RetValReg, 0, Reg.getNumber());
      IB->CreateAlignedStore(Val, Gep, llvm::MaybeAlign{8});
      return;
    }
    if (Reg.getClass() == 'a') {
      auto *Gep = IB->CreateConstGEP2_64(ArgRegFile_t, ArgRegFile, 0, Reg.getNumber());
      IB->CreateAlignedStore(Val, Gep, llvm::MaybeAlign{8});
      return;
    }
    if (Reg.getClass() == 't') {
      assert(FuncToRegFileMap.find(FuncName) != FuncToRegFileMap.end());
      auto RegFileOffset = FuncToRegFileMap[FuncName];
      auto *Gep = IB->CreateConstGEP2_64(TmpRegFile_t, TmpRegFile, 0, 
                                         RegFileOffset + Reg.getNumber());
      IB->CreateAlignedStore(Val, Gep, llvm::MaybeAlign{8});
      return;
    }
    utils::reportFatalError("Unknown reg class");
  }

private:
  void generateRet(Instruction &I, InstructionEnv &Env) {
    IB->CreateRetVoid();
  }

  void generateIncJump(Instruction &I, InstructionEnv &Env) {
    auto *Const1i64 = llvm::ConstantInt::get(IB->getInt64Ty(), 1);

    auto Reg = std::get<Register>(I.getArg(0));
    auto *RegToInc = generateRegValueLoad(Reg, Env.FuncName);
    auto ImmToCmpVal = std::get<Immidiate>(I.getArg(1));
    auto *ImmToCmp = llvm::ConstantInt::get(IB->getInt64Ty(), ImmToCmpVal);
    auto Label1Name = std::get<Label>(I.getArg(2));
    assert(Env.BBMap.find(Label1Name) != Env.BBMap.end());
    auto *Label1 = Env.BBMap[Label1Name];
    auto Label2Name = std::get<Label>(I.getArg(3));
    assert(Env.BBMap.find(Label2Name) != Env.BBMap.end());
    auto *Label2 = Env.BBMap[Label2Name];
    assert(I.getReturnValue());
    auto RegToStore = I.getReturnValue();

    auto *Val = IB->CreateAdd(RegToInc, Const1i64);
    generateStoreToReg(*RegToStore, Val, Env.FuncName);
    auto *Cond = IB->CreateICmpEQ(Val, ImmToCmp);

    IB->CreateCondBr(Cond, Label1, Label2);
  }

  void generateJumpIfDot(Instruction &I, InstructionEnv &Env) {
    auto *Const0i64 = llvm::ConstantInt::get(IB->getInt64Ty(), 0); 

    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtrI64 = generateRegValueLoad(StructPtrReg, Env.FuncName);
    auto ImmOffVal = std::get<Immidiate>(I.getArg(1));
    auto *ImmOff = llvm::ConstantInt::get(IB->getInt32Ty(), ImmOffVal);
    auto RegToCmp = std::get<Register>(I.getArg(2));
    auto *ValCmpWith = generateRegValueLoad(RegToCmp, Env.FuncName);
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
    auto *Cond = generateRegValueLoad(CondReg, Env.FuncName);
    auto Label1Name = std::get<Label>(I.getArg(1));
    assert(Env.BBMap.find(Label1Name) != Env.BBMap.end());
    auto *Label1 = Env.BBMap[Label1Name];
    auto Label2Name = std::get<Label>(I.getArg(2));
    assert(Env.BBMap.find(Label2Name) != Env.BBMap.end());
    auto *Label2 = Env.BBMap[Label2Name];

    auto *TruncatedCond = IB->CreateTrunc(Cond, IB->getInt1Ty());
    IB->CreateCondBr(TruncatedCond, Label1, Label2);
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

    initSubTypes();
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
  virtual void generateDataFlowInstruction(Instruction &I, InstructionEnv Env) = 0;
  virtual void initSubTypes() = 0;

public:
  virtual ~ControlFlowGenerator() = default;

  void generateInstruction(Instruction &I, InstructionEnv Env) override {
    if (generateControlFlowInstruction(I, Env))
      return;
    generateDataFlowInstruction(I, Env);
  } 
};

class PseudoGenerator final : public ControlFlowGenerator {
  llvm::FunctionType *Func0Args_t = nullptr;
  llvm::FunctionType *Func1Args_t = nullptr;
  llvm::FunctionType *Func2Args_t = nullptr;
  llvm::FunctionType *Func3Args_t = nullptr;
  llvm::FunctionType *Func4Args_t = nullptr;

  constexpr static auto RegFileStep = 32u;
  
  static uint64_t xorshift(uint64_t Seed) {
    Seed ^= Seed << 13;
    Seed ^= Seed >> 7;
    Seed ^= Seed << 17;
    return Seed; 
  }

  void generateLoadDotFiled(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_loadDotField", Func3Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtr = IB->getInt64(getRegisterID(StructPtrReg, Env.FuncName));
    auto ImmOffVal = std::get<Immidiate>(I.getArg(1));
    auto *ImmOff = IB->getInt64(ImmOffVal);
    auto Args = std::vector<llvm::Value *>{Val, StructPtr, ImmOff};

    IB->CreateCall(Func, Args);
  }

  void generateStoreDotField(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_storeDotField", Func3Args_t);

    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtr = IB->getInt64(getRegisterID(StructPtrReg, Env.FuncName));
    auto ImmOffVal = std::get<Immidiate>(I.getArg(1));
    auto *ImmOff = IB->getInt64(ImmOffVal);
    auto ValToStoreReg = std::get<Register>(I.getArg(2));
    auto *ValToStore = IB->getInt64(getRegisterID(ValToStoreReg, Env.FuncName));
    auto Args = std::vector<llvm::Value *>{StructPtr, ImmOff, ValToStore};

    IB->CreateCall(Func, Args);
  }

  void generateStoreDotFieldImm(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_storeDotFieldImm", Func3Args_t);

    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtr = IB->getInt64(getRegisterID(StructPtrReg, Env.FuncName));
    auto ImmOffVal = std::get<Immidiate>(I.getArg(1));
    auto *ImmOff = IB->getInt64(ImmOffVal);
    auto ImmToStoreVal = std::get<Immidiate>(I.getArg(2));
    auto *ImmToStore = IB->getInt64(ImmToStoreVal);
    auto Args = std::vector<llvm::Value *>{StructPtr, ImmOff, ImmToStore};

    IB->CreateCall(Func, Args);
  }

  void generateGetDotAddr(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_getDotAddr", Func3Args_t);

    assert(I.getReturnValue());
    auto AddrReg = *I.getReturnValue();
    auto *Addr = IB->getInt64(getRegisterID(AddrReg, Env.FuncName));
    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtr = IB->getInt64(getRegisterID(StructPtrReg, Env.FuncName));
    auto ValOffReg = std::get<Register>(I.getArg(1));
    auto *ValOff = IB->getInt64(getRegisterID(ValOffReg, Env.FuncName));
    auto Args = std::vector<llvm::Value *>{Addr, StructPtr, ValOff};

    IB->CreateCall(Func, Args);
  }

  void generateInitRgb(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_initRgb", Func2Args_t);

    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtr = IB->getInt64(getRegisterID(StructPtrReg, Env.FuncName));
    auto GVName = std::get<Label>(I.getArg(1));
    assert(GVMap.find(GVName) != GVMap.end());
    auto *Seed = GVMap[GVName];
    auto Args = std::vector<llvm::Value *>{StructPtr, Seed};
    //probably needs bitcast
    IB->CreateCall(Func, Args);
  }

  void generateLoadRgb(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_loadRgb", Func2Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtr = IB->getInt64(getRegisterID(StructPtrReg, Env.FuncName));
    auto Args = std::vector<llvm::Value *>{Val, StructPtr};

    IB->CreateCall(Func, Args);
  }

  void generateCmpTwo(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_cmpTwo", Func4Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto ValCmpWithReg = std::get<Register>(I.getArg(0));
    auto *ValCmpWith = IB->getInt64(getRegisterID(ValCmpWithReg, Env.FuncName));
    auto Val1Reg = std::get<Register>(I.getArg(1));
    auto *Val1 = IB->getInt64(getRegisterID(Val1Reg, Env.FuncName));
    auto Val2Reg = std::get<Register>(I.getArg(2));
    auto *Val2 = IB->getInt64(getRegisterID(Val2Reg, Env.FuncName));
    auto Args = std::vector<llvm::Value *>{Val, ValCmpWith, Val1, Val2};

    IB->CreateCall(Func, Args);
  }

  void generateNorm(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_norm", Func3Args_t);

    assert(I.getReturnValue());
    auto NormReg = *I.getReturnValue();
    auto *Norm = IB->getInt64(getRegisterID(NormReg, Env.FuncName));
    auto X1Reg = std::get<Register>(I.getArg(0));
    auto *X1 = IB->getInt64(getRegisterID(X1Reg, Env.FuncName));
    auto X2Reg = std::get<Register>(I.getArg(1));
    auto *X2 = IB->getInt64(getRegisterID(X2Reg, Env.FuncName));
    auto Args = std::vector<llvm::Value *>{Norm, X1, X2};

    IB->CreateCall(Func, Args);
  }

  void generateXorshift(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_xorshift", Func2Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto GVName = std::get<Label>(I.getArg(0));
    assert(GVMap.find(GVName) != GVMap.end());
    auto *Seed = GVMap[GVName];
    auto Args = std::vector<llvm::Value *>{Val, Seed};

    IB->CreateCall(Func, Args);
  }

  void generateCreateDots(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_createDots", Func2Args_t);

    assert(I.getReturnValue());
    auto PtrReg = *I.getReturnValue();
    auto *Ptr = IB->getInt64(getRegisterID(PtrReg, Env.FuncName));
    auto ImmSizeVal = std::get<Immidiate>(I.getArg(0));
    auto *ImmSize = IB->getInt64(ImmSizeVal);
    auto Args = std::vector<llvm::Value *>{Ptr, ImmSize};

    IB->CreateCall(Func, Args);
  }

  void generateAnd(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_and", Func3Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto ValInReg = std::get<Register>(I.getArg(0));
    auto *ValIn = IB->getInt64(getRegisterID(ValInReg, Env.FuncName));
    auto ImmVal = std::get<Immidiate>(I.getArg(1));
    auto *Imm = IB->getInt64(ImmVal);
    auto Args = std::vector<llvm::Value *>{Val, ValIn, Imm};

    IB->CreateCall(Func, Args);
  }

  void generateCmpEqImm(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_cmpEqImm", Func3Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto ValInReg = std::get<Register>(I.getArg(0));
    auto *ValIn = IB->getInt64(getRegisterID(ValInReg, Env.FuncName));
    auto ImmVal = std::get<Immidiate>(I.getArg(1));
    auto *Imm = IB->getInt64(ImmVal);
    auto Args = std::vector<llvm::Value *>{Val, ValIn, Imm};

    IB->CreateCall(Func, Args);
  }

  void generateCmpUGTImm(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_cmpUGTImm", Func3Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto ValInReg = std::get<Register>(I.getArg(0));
    auto *ValIn = IB->getInt64(getRegisterID(ValInReg, Env.FuncName));
    auto ImmVal = std::get<Immidiate>(I.getArg(1));
    auto *Imm = IB->getInt64(ImmVal);
    auto Args = std::vector<llvm::Value *>{Val, ValIn, Imm};

    IB->CreateCall(Func, Args);
  }

  void generateCmpUGT(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_cmpUGT", Func3Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto ValLhsReg = std::get<Register>(I.getArg(0));
    auto *ValLhs = IB->getInt64(getRegisterID(ValLhsReg, Env.FuncName));
    auto ValRhsReg = std::get<Register>(I.getArg(1));
    auto *ValRhs = IB->getInt64(getRegisterID(ValRhsReg, Env.FuncName));
    auto Args = std::vector<llvm::Value *>{Val, ValLhs, ValRhs};

    IB->CreateCall(Func, Args);
  }

  void generateLi(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_li", Func2Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto ImmVal = std::get<Immidiate>(I.getArg(0));
    auto *Imm = IB->getInt64(ImmVal);
    auto Args = std::vector<llvm::Value *>{Val, Imm};

    IB->CreateCall(Func, Args);
  }

  void generateMul(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_mul", Func3Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto ValLhsReg = std::get<Register>(I.getArg(0));
    auto *ValLhs = IB->getInt64(getRegisterID(ValLhsReg, Env.FuncName));
    auto ValRhsReg = std::get<Register>(I.getArg(1));
    auto *ValRhs = IB->getInt64(getRegisterID(ValRhsReg, Env.FuncName));
    auto Args = std::vector<llvm::Value *>{Val, ValLhs, ValRhs};

    IB->CreateCall(Func, Args);
  }

  void generateAdd(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_add", Func3Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto ValLhsReg = std::get<Register>(I.getArg(0));
    auto *ValLhs = IB->getInt64(getRegisterID(ValLhsReg, Env.FuncName));
    auto ValRhsReg = std::get<Register>(I.getArg(1));
    auto *ValRhs = IB->getInt64(getRegisterID(ValRhsReg, Env.FuncName));
    auto Args = std::vector<llvm::Value *>{Val, ValLhs, ValRhs};

    IB->CreateCall(Func, Args);
  }

  void generateSub(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_sub", Func3Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto ValLhsReg = std::get<Register>(I.getArg(0));
    auto *ValLhs = IB->getInt64(getRegisterID(ValLhsReg, Env.FuncName));
    auto ValRhsReg = std::get<Register>(I.getArg(1));
    auto *ValRhs = IB->getInt64(getRegisterID(ValRhsReg, Env.FuncName));
    auto Args = std::vector<llvm::Value *>{Val, ValLhs, ValRhs};

    IB->CreateCall(Func, Args);
  }

  void generateSelect(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_select", Func4Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto CondReg = std::get<Register>(I.getArg(0));
    auto *Cond = IB->getInt64(getRegisterID(CondReg, Env.FuncName));
    auto ValIn1Reg = std::get<Register>(I.getArg(1));
    auto *ValIn1 = IB->getInt64(getRegisterID(ValIn1Reg, Env.FuncName));
    auto ValIn2Reg = std::get<Register>(I.getArg(2));
    auto *ValIn2 = IB->getInt64(getRegisterID(ValIn2Reg, Env.FuncName));
    auto Args = std::vector<llvm::Value *>{Val, Cond, ValIn1, ValIn2};

    IB->CreateCall(Func, Args);
  }

  void generateMv(Instruction &I, InstructionEnv Env) {
    auto Func = M->getOrInsertFunction("do_mv", Func2Args_t);

    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto *Val = IB->getInt64(getRegisterID(ValReg, Env.FuncName));
    auto RegReg = std::get<Register>(I.getArg(0));
    auto *Reg = IB->getInt64(getRegisterID(RegReg, Env.FuncName));
    auto Args = std::vector<llvm::Value *>{Val, Reg};

    IB->CreateCall(Func, Args);
  }

  void generateDataFlowInstruction(Instruction &I, InstructionEnv Env) override {
    auto Name = I.getOpcode();
    if (Name == "loadDotField") {
      generateLoadDotFiled(I, Env);
      return;
    }
    if (Name == "storeDotField") {
      generateStoreDotField(I, Env);
      return;
    }
    if (Name == "storeDotFieldImm") {
      generateStoreDotFieldImm(I, Env);
      return;
    }
    if (Name == "getDotAddr") {
      generateGetDotAddr(I, Env);
      return;
    }
    if (Name == "initRgb") {
      generateInitRgb(I, Env);
      return;
    }
    if (Name == "loadRgb") {
      generateLoadRgb(I, Env);
      return;
    }
    if (Name == "cmpTwo") {
      generateCmpTwo(I, Env);
      return;
    }
    if (Name == "norm") {
      generateNorm(I, Env);
      return;
    }
    if (Name == "xorshift") {
      generateXorshift(I, Env);
      return;
    }
    if (Name == "createDots") {
      generateCreateDots(I, Env);
      return;
    }
    if (Name == "and") {
      generateAnd(I, Env);
      return;
    }
    if (Name == "cmpEqImm") {
      generateCmpEqImm(I, Env);
      return;
    }
    if (Name == "cmpUGTImm") {
      generateCmpUGTImm(I, Env);
      return;
    }
    if (Name == "cmpUGT") {
      generateCmpUGT(I, Env);
      return;
    }
    if (Name == "li") {
      generateLi(I, Env);
      return;
    }
    if (Name == "mul") {
      generateMul(I, Env);
      return;
    }
    if (Name == "add") {
      generateAdd(I, Env);
      return;
    }
    if (Name == "sub") {
      generateSub(I, Env);
      return;
    }
    if (Name == "select") {
      generateSelect(I, Env);
      return;
    }
    if (Name == "mv") {
      generateMv(I, Env);
      return;
    }
    utils::reportFatalError("Unknown instruction:");
  }

  void initSubTypes() override {
    auto *Void_t = llvm::Type::getVoidTy(*Ctx);
    auto *I64_t = llvm::Type::getInt64Ty(*Ctx);
    auto Args = std::vector<llvm::Type *>{Void_t};
    Func0Args_t = llvm::FunctionType::get(Void_t, Args, /*isVarArg*/ false);

    Args = std::vector<llvm::Type *>{I64_t};
    Func1Args_t = llvm::FunctionType::get(Void_t, Args, /*isVarArg*/ false);

    Args.push_back(I64_t);
    Func2Args_t = llvm::FunctionType::get(Void_t, Args, /*isVarArg*/ false);
    
    Args.push_back(I64_t);
    Func3Args_t = llvm::FunctionType::get(Void_t, Args, /*isVarArg*/ false);

    Args.push_back(I64_t);
    Func4Args_t = llvm::FunctionType::get(Void_t, Args, /*isVarArg*/ false);
  }

  static uint64_t *__TmpRegFilePtr;
  static uint64_t *__ArgsRegFilePtr;
  static uint64_t *__RetValuePtr;

  // Registers have classes, so this function makes general numeration for them
  size_t getRegisterID(Register Reg, const std::string &FuncName) {
    constexpr auto RetVaLRegFileBegin = 0ul;
    constexpr auto ArgsRegFileBegin = 1ul;
    constexpr auto TmpRegFileBegin = ArgsRegFileBegin + ArgsRegFileSize;
    auto BaseOffset = 0ull;

    // Registers:
    // r | a0, a1, a2, a3 | t0 ... t11 ; t0 .. t11 (another functions)

    switch (Reg.getClass()) {
    case 'r':
      BaseOffset = RetVaLRegFileBegin;
      break;
    case 'a':
      BaseOffset = ArgsRegFileBegin;
      break;
    case 't':
      assert(FuncToRegFileMap.find(FuncName) != FuncToRegFileMap.end());
      BaseOffset = TmpRegFileBegin + FuncToRegFileMap[FuncName];
      break;
    default:
      utils::reportFatalError("Unknown register class: " + 
                              std::string{Reg.getClass()});
    };
    return BaseOffset + Reg.getNumber();
  }

  static uint64_t &getRegVal(uint64_t RegID) {
    constexpr auto RetVaLRegFileBegin = 0ul;
    constexpr auto ArgsRegFileBegin = 1ul;
    constexpr auto TmpRegFileBegin = ArgsRegFileBegin + ArgsRegFileSize;

    if (RegID >= RetVaLRegFileBegin && RegID < ArgsRegFileBegin)
      return __RetValuePtr[0];
    if (RegID >= ArgsRegFileBegin && RegID < TmpRegFileBegin)
      return __ArgsRegFilePtr[RegID - ArgsRegFileBegin];
    auto TmpRegFilePos = RegID - TmpRegFileBegin;
    return __TmpRegFilePtr[TmpRegFilePos];
  }

  static void do_simFlush() {
    lib::simFlush();
  }

  static void do_simPutPixel() {
    auto XReg = __ArgsRegFilePtr[0];
    auto YReg = __ArgsRegFilePtr[1];
    auto RGBReg = __ArgsRegFilePtr[2];
    auto RGB = lib::RGB{};
    auto Bytes = utils::bit_cast<std::array<uint8_t, sizeof(RGBReg)>>(RGBReg);
    RGB.R = Bytes[0];
    RGB.G = Bytes[1];
    RGB.B = Bytes[2];
    lib::simPutPixel(XReg, YReg, RGB);
  }
 
  static void do_loadDotField(uint64_t Val, uint64_t StructPtr, uint64_t ImmOff) {
    auto *StructPtrVal = reinterpret_cast<uint64_t *>(getRegVal(StructPtr));
    assert(StructPtrVal);
    assert(ImmOff < 5);
    getRegVal(Val) = StructPtrVal[ImmOff];
  }

  static void do_storeDotField(uint64_t StructPtr, uint64_t ImmOff, uint64_t ValToStore) {
    auto *StructPtrVal = reinterpret_cast<uint64_t *>(getRegVal(StructPtr));
    assert(StructPtrVal);
    assert(ImmOff < 5);
    StructPtrVal[ImmOff] = getRegVal(ValToStore);
  }

  static void do_storeDotFieldImm(uint64_t StructPtr, uint64_t ImmOff, uint64_t ImmToStore) {
    auto *StructPtrVal = reinterpret_cast<uint64_t *>(getRegVal(StructPtr));
    assert(StructPtrVal);
    assert(ImmOff < 5);
    StructPtrVal[ImmOff] = ImmToStore;
  }

  static void do_getDotAddr(uint64_t Addr, uint64_t StructPtr, uint64_t VallOff) {
    static int Count = 0;
    Count++;
    auto *StructPtrVal = reinterpret_cast<Dot *>(getRegVal(StructPtr));
    assert(StructPtrVal);
    getRegVal(Addr) = reinterpret_cast<uint64_t>(StructPtrVal + getRegVal(VallOff));
  }

  // Seed is a pointer to global variable
  static void do_initRgb(uint64_t StructPtr, uint64_t Seed) {
    assert(Seed);
    auto &SeedVal = *reinterpret_cast<uint64_t *>(Seed);
    auto *DotPtr = reinterpret_cast<Dot *>(getRegVal(StructPtr));
    auto &RGB = DotPtr->Colour;
    SeedVal = xorshift(SeedVal);
    RGB.R = SeedVal;
    SeedVal = xorshift(SeedVal);
    RGB.G = SeedVal;
    SeedVal = xorshift(SeedVal);
    RGB.B = SeedVal;
  }

  static void do_loadRgb(uint64_t Val, uint64_t StructPtr) {
    assert(getRegVal(StructPtr));
    auto *DotPtr = reinterpret_cast<Dot *>(getRegVal(StructPtr));
    auto RGB = DotPtr->Colour;
    // bitcast analogue
    getRegVal(Val) = RGB.R + (RGB.G << 8) + (RGB.B << 16);
  }

  static void do_cmpTwo(uint64_t Val, uint64_t ValCmpWith, uint64_t Val1, uint64_t Val2) {
    getRegVal(Val) = (getRegVal(ValCmpWith) < getRegVal(Val1)) &&
                     (getRegVal(ValCmpWith) < getRegVal(Val2));
  }

  static void do_norm(uint64_t Norm, uint64_t X1, uint64_t X2) {
    getRegVal(Norm) = getRegVal(X1) * getRegVal(X1) + getRegVal(X2) * getRegVal(X2);
  }

  static void do_xorshift(uint64_t Val, uint64_t Seed) {
    auto &SeedVal = *reinterpret_cast<uint64_t *>(Seed);
    SeedVal = xorshift(SeedVal);
    getRegVal(Val) = SeedVal;
  }

  static void do_createDots(uint64_t Ptr, uint64_t ImmSize) {
    auto *DotsPtr = new Dot[ImmSize];
    getRegVal(Ptr) = reinterpret_cast<uint64_t>(DotsPtr);
  } 

  static void do_and(uint64_t Val, uint64_t ValIn, uint64_t Imm) {
    getRegVal(Val) = getRegVal(ValIn) & Imm;
  }

  static void do_cmpEqImm(uint64_t Val, uint64_t ValLhs, uint64_t Imm) {
    getRegVal(Val) = getRegVal(ValLhs) == Imm;
  }

  static void do_cmpUGTImm(uint64_t Val, uint64_t ValIn, uint64_t Imm) {
    getRegVal(Val) = getRegVal(ValIn) > Imm;
  }

  static void do_cmpUGT(uint64_t Val, uint64_t ValLhs, uint64_t ValRhs) {
    getRegVal(Val) = getRegVal(ValLhs) > getRegVal(ValRhs);
  }

  static void do_li(uint64_t Val, uint64_t Imm) {
    getRegVal(Val) = Imm;
  }

  static void do_mul(uint64_t Val, uint64_t Lhs, uint64_t Rhs) {;
    getRegVal(Val) = getRegVal(Lhs) * getRegVal(Rhs);
  }

  static void do_add(uint64_t Val, uint64_t Lhs, uint64_t Rhs) {
    getRegVal(Val) = getRegVal(Lhs) + getRegVal(Rhs);
  }

  static void do_sub(uint64_t Val, uint64_t Lhs, uint64_t Rhs) {
    getRegVal(Val) = getRegVal(Lhs) - getRegVal(Rhs);
  }

  static void do_select(uint64_t Val, uint64_t Cond, uint64_t ValIn1, uint64_t ValIn2) {
    // FIXME: check this cast
    getRegVal(Val) = getRegVal(Cond) % 2 ? getRegVal(ValIn1) : getRegVal(ValIn2);
  }

  static void do_mv(uint64_t Val, uint64_t Reg) {
    getRegVal(Val) = getRegVal(Reg);
  }

public:
  std::pair<IRToExecute::Mapping_t, IRToExecute::RegisterState> generateMapper() override {
    assert(FuncToRegFileMap.size());
    auto GeneralTmpRegFileSize = FuncToRegFileMap.size() * TmpRegFileSize;
    auto TmpRegFile = std::make_unique<uint64_t[]>(GeneralTmpRegFileSize);
    auto ArgsRegFile = std::make_unique<uint64_t[]>(ArgsRegFileSize);
    auto RetValue = std::make_unique<uint64_t[]>(1);
    
    __TmpRegFilePtr = TmpRegFile.get();
    __ArgsRegFilePtr = ArgsRegFile.get();
    __RetValuePtr = RetValue.get();
    auto Mapper = 
      [TmpRegFilePtr = __TmpRegFilePtr, 
       ArgsRegFilePtr = __ArgsRegFilePtr, 
       RetValuePtr = __RetValuePtr](const std::string &Name) -> void * {
        std::cout << "Mapping {" << Name << "}\n";
        if (Name == "tmpRegFile")
          return TmpRegFilePtr;
        if (Name == "argRegFile")
          return ArgsRegFilePtr;
        if (Name == "retValReg")
          return RetValuePtr;
        if (Name == "simFlush")
          return reinterpret_cast<void *>(&do_simFlush);
        if (Name == "simPutPixel")
          return reinterpret_cast<void *>(&do_simPutPixel);
        if (Name == "do_loadDotField")
          return reinterpret_cast<void *>(&do_loadDotField);
        if (Name == "do_storeDotField")
          return reinterpret_cast<void *>(&do_storeDotField);
        if (Name == "do_storeDotFieldImm")
          return reinterpret_cast<void *>(&do_storeDotFieldImm);
        if (Name == "do_getDotAddr")
          return reinterpret_cast<void *>(&do_getDotAddr);
        if (Name == "do_initRgb")
          return reinterpret_cast<void *>(&do_initRgb);
        if (Name == "do_loadRgb")
          return reinterpret_cast<void *>(&do_loadRgb);
        if (Name == "do_cmpTwo")
          return reinterpret_cast<void *>(&do_cmpTwo);
        if (Name == "do_norm")
          return reinterpret_cast<void *>(&do_norm);
        if (Name == "do_xorshift")
          return reinterpret_cast<void *>(&do_xorshift);
        if (Name == "do_createDots")
          return reinterpret_cast<void *>(&do_createDots);
        if (Name == "do_and")
          return reinterpret_cast<void *>(&do_and);
        if (Name == "do_cmpEqImm")
          return reinterpret_cast<void *>(&do_cmpEqImm);
        if (Name == "do_cmpUGTImm")
          return reinterpret_cast<void *>(&do_cmpUGTImm);
        if (Name == "do_cmpUGT")
          return reinterpret_cast<void *>(&do_cmpUGT);
        if (Name == "do_li")
          return reinterpret_cast<void *>(&do_li);
        if (Name == "do_mul")
          return reinterpret_cast<void *>(&do_mul);
        if (Name == "do_add")
          return reinterpret_cast<void *>(&do_add);
        if (Name == "do_sub")
          return reinterpret_cast<void *>(&do_sub);
        if (Name == "do_select")
          return reinterpret_cast<void *>(&do_select);
        if (Name == "do_mv")
          return reinterpret_cast<void *>(&do_mv);
        utils::reportFatalError("Unknown callback: " + Name);
        return nullptr; 
      };
    return {std::move(Mapper), IRToExecute::RegisterState{std::move(TmpRegFile), 
                                                          std::move(ArgsRegFile),
                                                          std::move(RetValue)}};
  }
};

uint64_t *PseudoGenerator::__TmpRegFilePtr = nullptr;
uint64_t *PseudoGenerator::__ArgsRegFilePtr = nullptr;
uint64_t *PseudoGenerator::__RetValuePtr = nullptr;

class RealGenerator final : public ControlFlowGenerator {
  static uint64_t *__TmpRegFilePtr;
  static uint64_t *__ArgsRegFilePtr;
  static uint64_t *__RetValuePtr;

  llvm::Function *xorshiftFunc = nullptr;

  void generateLoadDotFiled(Instruction &I, InstructionEnv Env) {
    auto *Const0i64 = llvm::ConstantInt::get(IB->getInt64Ty(), 0); 

    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtrI64 = generateRegValueLoad(StructPtrReg, Env.FuncName);
    auto ImmOffVal = std::get<Immidiate>(I.getArg(1));
    auto *ImmOff = llvm::ConstantInt::get(IB->getInt32Ty(), ImmOffVal);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();

    auto *StructPtr = IB->CreateBitCast(StructPtrI64, llvm::PointerType::getUnqual(Dot_t));
    auto Values = std::vector<llvm::Value *>{Const0i64, ImmOff};
    auto *Addr = IB->CreateInBoundsGEP(Dot_t, StructPtr, Values);
    auto *Val = IB->CreateAlignedLoad(IB->getInt64Ty(), Addr, llvm::MaybeAlign{8});
    generateStoreToReg(ValReg, Val, Env.FuncName);
  }

  void generateStoreDotField(Instruction &I, InstructionEnv Env) {
    auto *Const0i64 = llvm::ConstantInt::get(IB->getInt64Ty(), 0); 

    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtrI64 = generateRegValueLoad(StructPtrReg, Env.FuncName);
    auto ImmOffVal = std::get<Immidiate>(I.getArg(1));
    auto *ImmOff = llvm::ConstantInt::get(IB->getInt32Ty(), ImmOffVal);
    auto ValToStoreReg = std::get<Register>(I.getArg(2));
    auto *ValToStore = generateRegValueLoad(ValToStoreReg, Env.FuncName);

    auto *StructPtr = IB->CreateBitCast(StructPtrI64, llvm::PointerType::getUnqual(Dot_t));
    auto Values = std::vector<llvm::Value *>{Const0i64, ImmOff};
    auto *Ptr = IB->CreateInBoundsGEP(Dot_t, StructPtr, Values);
    IB->CreateAlignedStore(ValToStore, Ptr, llvm::MaybeAlign{8});
  }

  void generateStoreDotFieldImm(Instruction &I, InstructionEnv Env) {
    auto *Const0i64 = llvm::ConstantInt::get(IB->getInt64Ty(), 0); 

    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtrI64 = generateRegValueLoad(StructPtrReg, Env.FuncName);
    auto ImmOffVal = std::get<Immidiate>(I.getArg(1));
    auto *ImmOff = llvm::ConstantInt::get(IB->getInt32Ty(), ImmOffVal);
    auto ImmToStoreVal = std::get<Immidiate>(I.getArg(2));
    auto *ImmToStore = llvm::ConstantInt::get(IB->getInt64Ty(), ImmOffVal);

    auto *StructPtr = IB->CreateBitCast(StructPtrI64, llvm::PointerType::getUnqual(Dot_t));
    auto Values = std::vector<llvm::Value *>{Const0i64, ImmOff};
    auto *Ptr = IB->CreateInBoundsGEP(Dot_t, StructPtr, Values);
    IB->CreateAlignedStore(ImmToStore, Ptr, llvm::MaybeAlign{8});
  }

  void generateGetDotAddr(Instruction &I, InstructionEnv Env) {
    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtrI64 = generateRegValueLoad(StructPtrReg, Env.FuncName);
    auto VallOffReg= std::get<Register>(I.getArg(1));
    auto *ValOff =  generateRegValueLoad(VallOffReg, Env.FuncName);
    assert(I.getReturnValue());
    auto AddrReg = I.getReturnValue();

    auto *StructPtr = IB->CreateBitCast(StructPtrI64, llvm::PointerType::getUnqual(Dot_t));
    auto Values = std::vector<llvm::Value *>{ValOff};
    auto *Ptr = IB->CreateInBoundsGEP(Dot_t, StructPtr, Values);
    generateStoreToReg(*AddrReg, Ptr, Env.FuncName);
  }

  void generateInitRgb(Instruction &I, InstructionEnv Env) {
    auto StoreAlign = llvm::MaybeAlign{8};
    auto Const64i0 = llvm::ConstantInt::get(IB->getInt64Ty(), 0);
    auto Const32i5 = llvm::ConstantInt::get(IB->getInt32Ty(), 5);
    auto Const32i0 = llvm::ConstantInt::get(IB->getInt32Ty(), 0);
    auto Const32i1 = llvm::ConstantInt::get(IB->getInt32Ty(), 1);
    auto Const32i2 = llvm::ConstantInt::get(IB->getInt32Ty(), 2);
    
    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtrI64 = generateRegValueLoad(StructPtrReg, Env.FuncName);
    auto SeedName = std::get<Label>(I.getArg(1));
    assert(GVMap.find(SeedName) != GVMap.end());
    auto *Seed = GVMap[SeedName];
    auto Args = std::vector<llvm::Value *>{Seed};

    auto *StructPtr = IB->CreateBitCast(StructPtrI64, llvm::PointerType::getUnqual(Dot_t));

    auto *XorShift1 = IB->CreateCall(xorshiftFunc, Args);
    auto *Trunc1 = IB->CreateTrunc(XorShift1, IB->getInt8Ty());
    auto Values = std::vector<llvm::Value *>{Const64i0, Const32i5, Const32i0};
    auto *Gep6 = IB->CreateInBoundsGEP(Dot_t, StructPtr, Values);
    IB->CreateAlignedStore(Trunc1, Gep6, StoreAlign);

    auto *XorShift2 = IB->CreateCall(xorshiftFunc, Args);
    auto *Trunc2 = IB->CreateTrunc(XorShift2, IB->getInt8Ty());
    Values = std::vector<llvm::Value *>{Const64i0, Const32i5, Const32i1};
    auto *Gep7 = IB->CreateInBoundsGEP(Dot_t, StructPtr, Values);
    IB->CreateAlignedStore(Trunc2, Gep7, StoreAlign);

    auto *XorShift3 = IB->CreateCall(xorshiftFunc, Args);
    auto *Trunc3 = IB->CreateTrunc(XorShift3, IB->getInt8Ty());
    Values = std::vector<llvm::Value *>{Const64i0, Const32i5, Const32i2};
    auto *Gep8 = IB->CreateInBoundsGEP(Dot_t, StructPtr, Values);
    IB->CreateAlignedStore(Trunc3, Gep8, StoreAlign);
  }

  void generateLoadRgb(Instruction &I, InstructionEnv Env) {
    auto Const64i0 = llvm::ConstantInt::get(IB->getInt64Ty(), 0);
    auto Const32i5 = llvm::ConstantInt::get(IB->getInt32Ty(), 5);
    auto Int24_t = IB->getIntNTy(24);

    auto StructPtrReg = std::get<Register>(I.getArg(0));
    auto *StructPtrI64 = generateRegValueLoad(StructPtrReg, Env.FuncName);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();

    auto *StructPtr = IB->CreateBitCast(StructPtrI64, llvm::PointerType::getUnqual(Dot_t));
    auto Values = std::vector<llvm::Value *>{Const64i0, Const32i5};
    auto *Addr = IB->CreateInBoundsGEP(Dot_t, StructPtr, Values);
    auto *Bitcast = IB->CreateBitCast(Addr, llvm::PointerType::getUnqual(Int24_t));
    auto *Val = 
      IB->CreateAlignedLoad(llvm::PointerType::getUnqual(Int24_t), 
                            Bitcast, llvm::MaybeAlign{8});
    auto *ExtendedVal = IB->CreateZExt(Val, IB->getInt64Ty());
    generateStoreToReg(ValReg, ExtendedVal, Env.FuncName);
  }

  void generateCmpTwo(Instruction &I, InstructionEnv Env) {
    auto ValCmpWithReg = std::get<Register>(I.getArg(0));
    auto *ValCmpWith = generateRegValueLoad(ValCmpWithReg, Env.FuncName);
    auto Val1Reg = std::get<Register>(I.getArg(1));
    auto *Val1 = generateRegValueLoad(Val1Reg, Env.FuncName);
    auto Val2Reg = std::get<Register>(I.getArg(2));
    auto *Val2 = generateRegValueLoad(Val2Reg, Env.FuncName);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();

    auto *Icmp1 = IB->CreateICmpULT(ValCmpWith, Val1);
    auto *Icmp2 = IB->CreateICmpULT(ValCmpWith, Val2);
    auto *Val = IB->CreateAnd(Icmp1, Icmp2);
    auto *ExtendedVal = IB->CreateZExt(Val, IB->getInt64Ty());
    generateStoreToReg(ValReg, ExtendedVal, Env.FuncName);
  }

  void generateNorm(Instruction &I, InstructionEnv Env) {
    auto X1Reg = std::get<Register>(I.getArg(0));
    auto *X1 = generateRegValueLoad(X1Reg, Env.FuncName);
    auto X2Reg = std::get<Register>(I.getArg(1));
    auto *X2 = generateRegValueLoad(X2Reg, Env.FuncName);
    assert(I.getReturnValue());
    auto NormReg = *I.getReturnValue();
    

    auto *Mul1 = IB->CreateMul(X1, X1);
    auto *Mul2 = IB->CreateMul(X2, X2);
    auto *Norm = IB->CreateAdd(Mul2, Mul1);
    generateStoreToReg(NormReg, Norm, Env.FuncName);
  }

  void generateXorshift(Instruction &I, InstructionEnv Env) {
    auto SeedName = std::get<Label>(I.getArg(0));
    assert(GVMap.find(SeedName) != GVMap.end());
    auto *Seed = GVMap[SeedName];
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    auto Args = std::vector<llvm::Value *>{Seed};

    auto *Val = IB->CreateCall(xorshiftFunc, Args);
    generateStoreToReg(ValReg, Val, Env.FuncName);
  }

  void generateCreateDots(Instruction &I, InstructionEnv Env) {
    auto Const64i0 = llvm::ConstantInt::get(IB->getInt64Ty(), 0);

    auto ImmSizeVal = std::get<Immidiate>(I.getArg(0));
    auto *DotsArray_t = llvm::ArrayType::get(Dot_t, ImmSizeVal);
    assert(I.getReturnValue());
    auto PtrReg = *I.getReturnValue();

    auto *Alloca = IB->CreateAlloca(DotsArray_t);
    auto *Bitcast = IB->CreateBitCast(Alloca, IB->getInt8PtrTy());

    auto Values = std::vector<llvm::Value *>{Const64i0, Const64i0};
    auto *Ptr = IB->CreateInBoundsGEP(DotsArray_t, Alloca, Values);
    generateStoreToReg(PtrReg, Ptr, Env.FuncName);
  }

  void generateAnd(Instruction &I, InstructionEnv Env) {
    auto ValInReg = std::get<Register>(I.getArg(0));
    auto *ValIn = generateRegValueLoad(ValInReg, Env.FuncName);
    auto ImmVal = std::get<Immidiate>(I.getArg(1));
    auto *Imm = llvm::ConstantInt::get(IB->getInt64Ty(), ImmVal);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();

    auto *Val = IB->CreateAnd(ValIn, Imm);
    generateStoreToReg(ValReg, Val, Env.FuncName);
  }

  void generateCmpEqImm(Instruction &I, InstructionEnv Env) {
    auto ValInReg = std::get<Register>(I.getArg(0));
    auto *ValIn = generateRegValueLoad(ValInReg, Env.FuncName);
    auto ImmVal = std::get<Immidiate>(I.getArg(1));
    auto *Imm = llvm::ConstantInt::get(IB->getInt64Ty(), ImmVal);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();

    auto *Val = IB->CreateICmpEQ(ValIn, Imm);
    auto *ExtendedVal = IB->CreateZExt(Val, IB->getInt64Ty());
    generateStoreToReg(ValReg, ExtendedVal, Env.FuncName);
  }

  void generateCmpUGTImm(Instruction &I, InstructionEnv Env) {
    auto ValInReg = std::get<Register>(I.getArg(0));
    auto *ValIn = generateRegValueLoad(ValInReg, Env.FuncName);
    auto ImmVal = std::get<Immidiate>(I.getArg(1));
    auto *Imm = llvm::ConstantInt::get(IB->getInt64Ty(), ImmVal);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();

    auto *Val = IB->CreateICmpUGT(ValIn, Imm);
    auto *ExtendedVal = IB->CreateZExt(Val, IB->getInt64Ty());
    generateStoreToReg(ValReg, ExtendedVal, Env.FuncName);
  }

  void generateCmpUGT(Instruction &I, InstructionEnv Env) {
    auto ValLhsReg = std::get<Register>(I.getArg(0));
    auto *ValLhs = generateRegValueLoad(ValLhsReg, Env.FuncName);
    auto ValRhsReg = std::get<Register>(I.getArg(1));
    auto *ValRhs = generateRegValueLoad(ValRhsReg, Env.FuncName);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();

    auto *Val = IB->CreateICmpUGT(ValLhs, ValRhs);
    auto *ExtendedVal = IB->CreateZExt(Val, IB->getInt64Ty());
    generateStoreToReg(ValReg, ExtendedVal, Env.FuncName);
  }

  void generateLi(Instruction &I, InstructionEnv Env) {
    auto ImmVal = std::get<Immidiate>(I.getArg(0));
    auto *Imm = llvm::ConstantInt::get(IB->getInt64Ty(), ImmVal);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();

    generateStoreToReg(ValReg, Imm, Env.FuncName);
  }

  void generateMul(Instruction &I, InstructionEnv Env) {
    auto LhsReg = std::get<Register>(I.getArg(0));
    auto *Lhs = generateRegValueLoad(LhsReg, Env.FuncName);
    auto RhsReg = std::get<Register>(I.getArg(1));
    auto *Rhs = generateRegValueLoad(RhsReg, Env.FuncName);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();

    auto *Val = IB->CreateMul(Lhs, Rhs);
    generateStoreToReg(ValReg, Val, Env.FuncName);
  }

  void generateAdd(Instruction &I, InstructionEnv Env) {
    auto LhsReg = std::get<Register>(I.getArg(0));
    auto *Lhs = generateRegValueLoad(LhsReg, Env.FuncName);
    auto RhsReg = std::get<Register>(I.getArg(1));
    auto *Rhs = generateRegValueLoad(RhsReg, Env.FuncName);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();

    auto *Val = IB->CreateAdd(Lhs, Rhs);
    generateStoreToReg(ValReg, Val, Env.FuncName);
  }

  void generateSub(Instruction &I, InstructionEnv Env) {
    auto LhsReg = std::get<Register>(I.getArg(0));
    auto *Lhs = generateRegValueLoad(LhsReg, Env.FuncName);
    auto RhsReg = std::get<Register>(I.getArg(1));
    auto *Rhs = generateRegValueLoad(RhsReg, Env.FuncName);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();

    auto *Val = IB->CreateSub(Lhs, Rhs);
    generateStoreToReg(ValReg, Val, Env.FuncName);
  }

  void generateSelect(Instruction &I, InstructionEnv Env) {
    auto CondReg = std::get<Register>(I.getArg(0));
    auto *Cond = generateRegValueLoad(CondReg, Env.FuncName);
    auto ValIn1Reg = std::get<Register>(I.getArg(1));
    auto *ValIn1 = generateRegValueLoad(ValIn1Reg, Env.FuncName);
    auto ValIn2Reg = std::get<Register>(I.getArg(2));
    auto *ValIn2 = generateRegValueLoad(ValIn2Reg, Env.FuncName);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();
    
    auto *TruncatedCond = IB->CreateTrunc(Cond, IB->getInt1Ty());
    auto *Val = IB->CreateSelect(TruncatedCond, ValIn1, ValIn2);
    generateStoreToReg(ValReg, Val, Env.FuncName);
  }

  void generateMv(Instruction &I, InstructionEnv Env) {
    auto RegReg = std::get<Register>(I.getArg(0));
    auto *Reg = generateRegValueLoad(RegReg, Env.FuncName);
    assert(I.getReturnValue());
    auto ValReg = *I.getReturnValue();

    generateStoreToReg(ValReg, Reg, Env.FuncName);
  }

  void generateDataFlowInstruction(Instruction &I, InstructionEnv Env) override {
    auto Name = I.getOpcode();
    if (Name == "loadDotField") {
      generateLoadDotFiled(I, Env);
      return;
    }
    if (Name == "storeDotField") {
      generateStoreDotField(I, Env);
      return;
    }
    if (Name == "storeDotFieldImm") {
      generateStoreDotFieldImm(I, Env);
      return;
    }
    if (Name == "getDotAddr") {
      generateGetDotAddr(I, Env);
      return;
    }
    if (Name == "initRgb") {
      generateInitRgb(I, Env);
      return;
    }
    if (Name == "loadRgb") {
      generateLoadRgb(I, Env);
      return;
    }
    if (Name == "cmpTwo") {
      generateCmpTwo(I, Env);
      return;
    }
    if (Name == "norm") {
      generateNorm(I, Env);
      return;
    }
    if (Name == "xorshift") {
      generateXorshift(I, Env);
      return;
    }
    if (Name == "createDots") {
      generateCreateDots(I, Env);
      return;
    }
    if (Name == "and") {
      generateAnd(I, Env);
      return;
    }
    if (Name == "cmpEqImm") {
      generateCmpEqImm(I, Env);
      return;
    }
    if (Name == "cmpUGTImm") {
      generateCmpUGTImm(I, Env);
      return;
    }
    if (Name == "cmpUGT") {
      generateCmpUGT(I, Env);
      return;
    }
    if (Name == "li") {
      generateLi(I, Env);
      return;
    }
    if (Name == "mul") {
      generateMul(I, Env);
      return;
    }
    if (Name == "add") {
      generateAdd(I, Env);
      return;
    }
    if (Name == "sub") {
      generateSub(I, Env);
      return;
    }
    if (Name == "select") {
      generateSelect(I, Env);
      return;
    }
    if (Name == "mv") {
      generateMv(I, Env);
      return;
    }
    utils::reportFatalError("Unknown instruction:");
  }

  static void do_simFlush() {
    lib::simFlush();
  }

  static void do_simPutPixel() {
    auto XReg = __ArgsRegFilePtr[0];
    auto YReg = __ArgsRegFilePtr[1];
    auto RGBReg = __ArgsRegFilePtr[2];
    auto RGB = lib::RGB{};
    auto Bytes = utils::bit_cast<std::array<uint8_t, sizeof(RGBReg)>>(RGBReg);
    RGB.R = Bytes[0];
    RGB.G = Bytes[1];
    RGB.B = Bytes[2];
    lib::simPutPixel(XReg, YReg, RGB);
  }

  llvm::Function *generateXorShiftFunction() {
    auto *xorshift_t = 
      llvm::FunctionType::get(IB->getInt64Ty(), 
                              llvm::PointerType::getUnqual(IB->getInt64Ty()),
                             /*isVarArg*/ false);
    auto *xorshift =
      llvm::Function::Create(xorshift_t, llvm::Function::ExternalLinkage, "xorshift", *M);
    auto *BB = llvm::BasicBlock::Create(*Ctx, "Entry", xorshift);
    IB->SetInsertPoint(BB);

    auto SeedAlign = llvm::MaybeAlign{8};
    auto *Const13 = llvm::ConstantInt::get(IB->getInt64Ty(), 13);
    auto *Const7 = llvm::ConstantInt::get(IB->getInt64Ty(), 7);
    auto *Const17 = llvm::ConstantInt::get(IB->getInt64Ty(), 17);
    auto *Seed = xorshift->getArg(0);

    auto *LoadInstr = 
      IB->CreateAlignedLoad(IB->getInt64Ty(), Seed, SeedAlign, /*isVolatile*/ false);
    auto *Shl1 = IB->CreateShl(LoadInstr, Const13);
    auto *Xor1 = IB->CreateXor(Shl1, LoadInstr);
    auto *Lshr = IB->CreateLShr(Xor1, Const7);
    auto *Xor2 = IB->CreateXor(Lshr, Xor1);
    auto *Shl2 = IB->CreateShl(Xor2, Const17);
    auto *Xor3 = IB->CreateXor(Shl2, Xor2);
    IB->CreateAlignedStore(Xor3, Seed, SeedAlign);

    IB->CreateRet(Xor3);
    return xorshift;
  }

  void initSubTypes() override {
    xorshiftFunc = generateXorShiftFunction();
  }

public:
  std::pair<IRToExecute::Mapping_t, IRToExecute::RegisterState> generateMapper() override {
    assert(FuncToRegFileMap.size());
    auto GeneralTmpRegFileSize = FuncToRegFileMap.size() * TmpRegFileSize;
    auto TmpRegFile = std::make_unique<uint64_t[]>(GeneralTmpRegFileSize);
    auto ArgsRegFile = std::make_unique<uint64_t[]>(ArgsRegFileSize);
    auto RetValue = std::make_unique<uint64_t[]>(1);
    
    __TmpRegFilePtr = TmpRegFile.get();
    __ArgsRegFilePtr = ArgsRegFile.get();
    __RetValuePtr = RetValue.get();
    auto Mapper = 
      [TmpRegFilePtr = __TmpRegFilePtr,
       ArgsRegFilePtr = __ArgsRegFilePtr, 
       RetValuePtr = __RetValuePtr](const std::string &Name) -> void * {
        std::cout << "Mapping {" << Name << "}\n";
        if (Name == "argRegFile")
          return ArgsRegFilePtr;
        if (Name == "tmpRegFile")
          return TmpRegFilePtr;
        if (Name == "retValReg")
          return RetValuePtr;
        if (Name == "simFlush")
          return reinterpret_cast<void *>(&do_simFlush);
        if (Name == "simPutPixel")
          return reinterpret_cast<void *>(&do_simPutPixel);
        utils::reportFatalError("Unknown callback: " + Name);
        return nullptr; 
      };
    return {std::move(Mapper), IRToExecute::RegisterState{std::move(TmpRegFile), 
                                                          std::move(ArgsRegFile),
                                                          std::move(RetValue)}};
  }
};

uint64_t *RealGenerator::__TmpRegFilePtr = nullptr;
uint64_t *RealGenerator::__ArgsRegFilePtr = nullptr;
uint64_t *RealGenerator::__RetValuePtr = nullptr;

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
  auto IR = Gen.generateIR(Code);
  auto [Mapper, RegFile] = Gen.generateMapper();
  return {std::move(IR), std::move(Mapper), 
          std::move(RegFile), Gen.getStartFunc()};
}

IRToExecute makeLLVMIR(assembler::Code &Code) {
  auto Gen = IR::RealGenerator{};
  auto IR = Gen.generateIR(Code);
  auto [Mapper, RegFile] = Gen.generateMapper();
  return {std::move(IR), std::move(Mapper), 
          std::move(RegFile), Gen.getStartFunc()};
}

} // namespace translator