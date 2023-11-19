#pragma once

#include "Utils.h"

#include <vector>
#include <string>
#include <variant>
#include <algorithm>

namespace assembler {

class Register {
  std::string Name;

  std::vector<char> AllowedClasses{'x'};

public:
  Register(const std::string &RegName) : Name{RegName} {
    if (RegName.empty())
      utils::reportFatalError("Empty reg name");
    auto RegClass = RegName.front();
    auto RegClassIt = 
      std::find(AllowedClasses.begin(), AllowedClasses.end(), RegClass);
    if (RegClassIt == AllowedClasses.end())
      utils::reportFatalError("Unknown register class");
  }
};

using Label = std::string;
using Immidiate = size_t;

using Argument = std::variant<Register, Immidiate, Label>;

class Instruction {
  std::string Name;
  std::vector<Argument> Args;

  template <typename It>
  Instruction(const std::string &Name, It ArgsBeg, It ArgsEnd) : Name{Name},
                                                                 Args{ArgsBeg, ArgsEnd} {}

public:
  static Instruction createInstruction(const std::string &Name, 
                                       const std::string &Arg1, 
                                       const std::string &Arg2);
};

class BasicBlock {
  std::string Label;
  std::vector<Instruction> Instructions;

public:
  template <typename It>
  BasicBlock(const std::string &Label, It IBeg, It IEnd) : Label{Label}, 
                                                           Instructions{IBeg, IEnd} {}
};  

class Code {
  std::string StartLabel;
  std::vector<BasicBlock> Blocks; 

public:
  
};

} // namespace assembler