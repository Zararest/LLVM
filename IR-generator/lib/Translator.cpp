#include "../include/Translator.h"
#include "../include/Assembler.h"

#include <algorithm>

#define MAKE_INSTR(name)                                     \
  if (!std::holds_alternative<Token::Word>(GlobalIt->Value)) \
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
    if (!std::holds_alternative<Token::Word>(GlobalIt->Value))
      return false;
    auto Name = GlobalIt->getName();
    if (Name == "start") 
      return false;

    GlobalIt++;
    auto Val = utils::stoull(GlobalIt->getName());
    checkIterator();
    GlobalIt++;
    DEBUG_EXPR(std::cout << "parsed constant" + GlobalIt->getName() << std::endl);
    Cfg.addGlobal(Global{}.addName(Name).addInitVal(Val));
    return true;
  }

  bool parseStart(GlobalConfig &Cfg) {
    if (!std::holds_alternative<Token::Word>(GlobalIt->Value))
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
    I.addArgument(utils::stoull(GlobalIt->getName()));
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
    return true;
  }

  bool parseRet(BasicBlock &BB) {
    MAKE_INSTR("ret");
    return true;
  }

  bool parseCmpEqImm(BasicBlock &BB) {
    MAKE_INSTR("cmpEqImm");
    parseReg(Instr);
    parseImm(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseBrCond(BasicBlock &BB) {
    MAKE_INSTR("brCond");
    parseReg(Instr);
    parseLabel(Instr);
    parseLabel(Instr);
    return true;
  }

  bool parseBr(BasicBlock &BB) {
    MAKE_INSTR("br");
    parseLabel(Instr);
    return true;
  }

  bool parseCall(BasicBlock &BB) {
    MAKE_INSTR("call");
    parseLabel(Instr);
    return true;
  }

  bool parseCmpUGTImm(BasicBlock &BB) {
    MAKE_INSTR("cmpUGTImm");
    parseReg(Instr);
    parseImm(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseCmpUGT(BasicBlock &BB) {
    MAKE_INSTR("cmpUGT");
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseLi(BasicBlock &BB) {
    MAKE_INSTR("li");
    parseImm(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseMul(BasicBlock &BB) {
    MAKE_INSTR("mul");
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseAdd(BasicBlock &BB) {
    MAKE_INSTR("add");
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseSub(BasicBlock &BB) {
    MAKE_INSTR("sub");
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseSelect(BasicBlock &BB) {
    MAKE_INSTR("select");
    parseReg(Instr);
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseMv(BasicBlock &BB) {
    MAKE_INSTR("mv");
    parseReg(Instr);
    parseRetVal(Instr);
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
    return true;
  } 

  bool parseLoadDotField(BasicBlock &BB) {
    MAKE_INSTR("loadDotField");
    parseReg(Instr);
    parseImm(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseStoreDotField(BasicBlock &BB) {
    MAKE_INSTR("storeDotField");
    parseReg(Instr);
    parseImm(Instr);
    parseReg(Instr);
    return true;
  }

  bool parseStoreDotFieldImm(BasicBlock &BB) {
    MAKE_INSTR("storeDotFieldImm");
    parseReg(Instr);
    parseImm(Instr);
    parseImm(Instr);
    return true;
  }

  bool parseJumpIfDot(BasicBlock &BB) {
    MAKE_INSTR("jumpIfDot");
    parseReg(Instr);
    parseImm(Instr);
    parseReg(Instr);
    parseLabel(Instr);
    parseLabel(Instr);
    return true;
  }

  bool parseGetDotAddr(BasicBlock &BB) {
    MAKE_INSTR("getDotAddr");
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseInitRgb(BasicBlock &BB) {
    MAKE_INSTR("initRgb");
    parseReg(Instr);
    parseLabel(Instr);
    return true;
  }

  bool parseLoadRgb(BasicBlock &BB) {
    MAKE_INSTR("loadRgb");
    parseReg(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseCmpTwo(BasicBlock &BB) {
    MAKE_INSTR("cmpTwo");
    parseReg(Instr);
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseNorm(BasicBlock &BB) {
    MAKE_INSTR("norm");
    parseReg(Instr);
    parseReg(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseXorshift(BasicBlock &BB) {
    MAKE_INSTR("xorshift");
    parseLabel(Instr);
    parseRetVal(Instr);
    return true;
  }

  bool parseCreateDots(BasicBlock &BB) {
    MAKE_INSTR("createDots");
    parseImm(Instr);
    parseRetVal(Instr);
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
    if (!std::holds_alternative<Token::Label>(GlobalIt->Value))
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
    return HadProgress;
  }

  bool parseFunction(Code &Code) {
    if (!std::holds_alternative<Token::Function>(GlobalIt->Value))
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

} // namespace

std::vector<Token> tokenize(std::string Program) {
  removeComments(Program);
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
      Res.emplace_back(getToken({WordBeg, WordEnd}));
    WordBeg = std::find_if(WordEnd, Program.end(), NotDelim);
  }

  return Res;
}

Code parse(std::vector<Token> Program) {
  return Parser{}.parse(Program);
}

} // namespace translator