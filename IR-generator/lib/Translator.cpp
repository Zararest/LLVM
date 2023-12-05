#include "../include/Translator.h"
#include "../include/Assembler.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <unordered_map>

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
using BBMap_t = std::unordered_map<std::string, llvm::BasicBlock *>;

class Generator {
protected:
  std::unique_ptr<llvm::Module> M;
  std::unique_ptr<llvm::LLVMContext> Ctx;
  std::unique_ptr<llvm::IRBuilder<>> IB;

  std::unordered_map<std::string, llvm::Function*> FuncMap;
  llvm::Function *Start;
  
  void generateFunctions(Code &Code) {
    auto GlobalCfg = Code.getGlobal();
    if (!GlobalCfg.hasStart())
      utils::reportFatalError("No start function");
    
    auto StartFuncName = GlobalCfg.getStart();

    for (auto &F : Code.getFunctions()) {
      auto *FuncType = llvm::FunctionType::get(IB->getVoidTy(), /*isVarArg*/ false);
      auto FuncName = F.getName();
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

  void generateInBB(BasicBlock &BB, BBMap_t &BBMap) {
    auto BBName = BB.getLabel();
    assert(BBMap.find(BBName) != BBMap.end());
    auto *LLVMBB = BBMap[BBName];
    assert(LLVMBB);
    IB->SetInsertPoint(LLVMBB);

    for (auto &I : BB.getInstructions())
      generateInstruction(I, BBMap);
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
      generateInBB(BB, BBMap);
  }

  virtual void generateInstruction(Instruction &I, BBMap_t &BBMap) = 0;

public: 
  virtual ~Generator() = default;

  IRToExecute::IR_t generateIR(Code &Code) {
    Ctx = std::make_unique<llvm::LLVMContext>();
    M = std::make_unique<llvm::Module>("Pseudo-IR", *Ctx);
    IB = std::make_unique<llvm::IRBuilder<>>(*Ctx);

    generateFunctions(Code);
    generateGlobals(Code);
    
    for (auto &F : Code.getFunctions())
      generateInFunction(F);
    M->print(llvm::outs(), nullptr);
    return {std::move(M), std::move(Ctx)};
  }

  llvm::Function *getStartFunc() { return Start; }

  virtual 
  std::pair<IRToExecute::Mapping_t, IRToExecute::RegFile_t> generateMapper() = 0;
};


class PseudoGenerator final : public Generator {  
  void generateRet(Instruction &I, BBMap_t &BBMap) {
    IB->CreateRetVoid();
  }

  void generateIncJump(Instruction &I, BBMap_t &BBMap) {
    //!!!!
    auto BrLabel = std::get<Label>(I.getArg(2));
    IB->CreateBr(BBMap[BrLabel]);
  }

  void generateJumpIfDot(Instruction &I, BBMap_t &BBMap) {
    auto BrLabel = std::get<Label>(I.getArg(3));
    IB->CreateBr(BBMap[BrLabel]);
  }

  void generateBrCond(Instruction &I, BBMap_t &BBMap) {
    auto BrLabel = std::get<Label>(I.getArg(1));
    IB->CreateBr(BBMap[BrLabel]);
  }

  void generateBr(Instruction &I, BBMap_t &BBMap) {
    auto BrLabel = std::get<Label>(I.getArg(0));
    IB->CreateBr(BBMap[BrLabel]);
  }

public:
  void generateInstruction(Instruction &I, BBMap_t &BBMap) override {
    auto Name = I.getOpcode();
    if (Name == "ret")
      generateRet(I, BBMap);
    if (Name == "incJump")
      generateIncJump(I, BBMap);
    if (Name == "jumpIfDot")
      generateJumpIfDot(I, BBMap);
    if (Name == "brCond")
      generateBrCond(I, BBMap);
    if (Name == "br")
      generateBr(I, BBMap);
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