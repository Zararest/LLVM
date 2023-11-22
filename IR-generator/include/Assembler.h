#pragma once

#include "Utils.h"

#include <vector>
#include <string>
#include <variant>
#include <algorithm>
#include <array>

namespace assembler {

class Register {
  char Class;
  uint64_t Number;

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

  Register addNumber(uint64_t Regnum) {
    Number = Regnum;
    return *this;

  }
};

using Label = std::string;
using Immidiate = uint64_t;
using Argument = std::variant<Register, Immidiate, Label>;

class Instruction {
  std::string Opcode;
  std::optional<Register> ReturnValue;
  std::vector<Argument> Args;

public:
  Instruction() = default;

  Instruction addOpcode(std::string OpcodeIn) {
    Opcode = Opcode;
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
};

class Global {
  std::string Name;
  uint64_t InitVal;

public: 
  Global() = default;

  Global addName(std::string NameIn) {
    Name = std::move(NameIn);
    return *this;
  }

  Global addInitVal(uint64_t InitValIn) {
    InitVal = InitValIn;
    return *this;
  }
};

class GlobalConfig {
  std::string Start;
  std::vector<Global> Globals;

public:
  GlobalConfig() = default;

  GlobalConfig addStart(std::string Start) {
    Start = std::move(Start);
    return *this;
  }

  GlobalConfig addGlobal(Global Glob) {
    Globals.emplace_back(std::move(Glob));
    return *this;
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
};

} // namespace assembler