#pragma once

#include "Utils.h"

#include <vector>
#include <string>
#include <variant>
#include <algorithm>
#include <array>
#include <iostream>

namespace assembler {

class Register {
  char Class;
  int64_t Number;

  static constexpr std::array AllowedClasses{'a', 't', 'r'};

public:
  Register() = default;

  Register(const std::string &Reg) {
    if (Reg.empty())
      utils::reportFatalError("Empty reg name");
    Class = Reg.front();
    if (std::find(AllowedClasses.begin(), AllowedClasses.end(), Class) 
        == AllowedClasses.end())
      utils::reportFatalError("Unknown register class");
    auto NumStr = std::string{std::next(Reg.begin()), Reg.end()};
    Number = std::atol(NumStr.c_str());
    if (Number == 0 && NumStr != "0")
      utils::reportFatalError("Invalid register name: " + Reg);
  }

  Register addClass(char ClassIn) {
    Class = ClassIn;
    return *this;
  }

  Register addNumber(int64_t Regnum) {
    Number = Regnum;
    return *this;
  }

  auto getClass() { return Class; }
  auto getNumber() { return Number; }

  void dump(std::ostream &S) {
    S << Class << Number;
  }
};

using Label = std::string;
using Immidiate = int64_t;
using Argument = std::variant<Register, Immidiate, Label>;

class Instruction {
  std::string Opcode;
  std::optional<Register> ReturnValue;
  std::vector<Argument> Args;

  void dumpArg(Argument &Arg, std::ostream &S) {
    if (std::holds_alternative<Register>(Arg)) {
      std::get<Register>(Arg).dump(S);
      return;
    }
    if (std::holds_alternative<Immidiate>(Arg)) {
      S << std::get<Immidiate>(Arg);
      return;
    }
    if (std::holds_alternative<Label>(Arg)) {
      S << std::get<Label>(Arg);
      return;
    }
  }

public:
  Instruction() = default;

  Instruction addOpcode(std::string OpcodeIn) {
    Opcode = OpcodeIn;
    return *this;
  }

  Instruction addReturnValue(Register Reg) {
    ReturnValue = std::move(Reg);
    return *this;
  }

  Instruction addArgument(Argument Arg) {
    Args.emplace_back(std::move(Arg));
    return *this;
  }

  auto getOpcode() { return Opcode; }
  auto getReturnValue() { return ReturnValue; }
  auto getArgs() { return utils::makeRange(Args.begin(), Args.end()); }

  void dump(std::ostream &S) {
    S << Opcode << " ";
    for (auto &Arg : Args) {
      dumpArg(Arg, S);
      S << ", ";
    }
    if (ReturnValue) {
      S << "-> ";
      ReturnValue->dump(S);
    }
    S << std::endl;
  }
};


class BasicBlock {
  Label BBLabel;
  std::vector<Instruction> Instructions;

public:
  BasicBlock() = default;

  BasicBlock addLabel(Label LabelIn) {
    BBLabel = LabelIn;
    return *this;
  }

  BasicBlock addInstruction(Instruction Instr) {
    Instructions.emplace_back(std::move(Instr));
    return *this;
  }

  auto getLabel() { return BBLabel; }
  auto getInstructions() { 
    return utils::makeRange(Instructions.begin(), Instructions.end()); 
  }

  void dump(std::ostream &S) {
    S << BBLabel << ":" << std::endl;
    for (auto &I : Instructions) {
      S << "\t";
      I.dump(S);
    }
  }
};  

class Function {
  std::string Name;
  std::vector<BasicBlock> Blocks; 

public:
  Function() = default;

  Function addName(std::string NameIn) {
    Name = std::move(NameIn);
    return *this;
  }

  Function addBlock(BasicBlock Block)  {
    Blocks.emplace_back(std::move(Block));
    return *this;
  }

  auto getName() { return Name; }
  auto getBlocks() { return utils::makeRange(Blocks.begin(), Blocks.end()); }

  void dump(std::ostream &S) {
    S << "<" << Name << ">" << std::endl;
    for (auto &BB : Blocks)
      BB.dump(S);
  }
};

class Global {
  std::string Name;
  int64_t InitVal;

public: 
  Global() = default;

  Global addName(std::string NameIn) {
    Name = std::move(NameIn);
    return *this;
  }

  Global addInitVal(int64_t InitValIn) {
    InitVal = InitValIn;
    return *this;
  }

  auto getName() { return Name; }
  auto getInitVal() { return InitVal; }

  void dump(std::ostream &S) {
    S << Name << " " << InitVal << std::endl;
  }
};

class GlobalConfig {
  std::string Start;
  std::vector<Global> Globals;

public:
  GlobalConfig() = default;

  GlobalConfig addStart(std::string StartIn) {
    Start = std::move(StartIn);
    return *this;
  }

  GlobalConfig addGlobal(Global Glob) {
    Globals.emplace_back(std::move(Glob));
    return *this;
  }

  auto getStart() { return Start; }
  auto getGlobals() { return utils::makeRange(Globals.begin(), Globals.end()); }

  bool hasStart() {
    return !Start.empty();
  }

  void dump(std::ostream &S) {
    S << "start: " << Start << "\n" << std::endl;
    for (auto &G : Globals)
      G.dump(S);
  }
};

class Code {
  GlobalConfig Global;
  std::vector<Function> Functions; 

public:
  Code() = default; 

  Code addGlobalConf(GlobalConfig Cfg) {
    Global = std::move(Cfg);
    return *this;
  }

  Code addFunction(Function Func) {
    Functions.emplace_back(std::move(Func));
    return *this;
  }

  auto getGlobal() { return Global; }
  auto getFunctions() { 
    return utils::makeRange(Functions.begin(), Functions.end()); 
  }

  void dump(std::ostream &S) {
    Global.dump(S);
    S << std::endl;
    for (auto &F : Functions) {
      F.dump(S);
      S << std::endl;
    }
  }
};

} // namespace assembler