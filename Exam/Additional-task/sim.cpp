//  CPU INSTRUCTION FORMATS:
//  Registers: 16 registers (32 bits) R0-R15
//
//  FF FF FF FF
//        ^^ ^^ - R3/IMM - third register or immediate
//      ^       - R2     - second register
//     ^        - R1     - first register
//  ^^          - opcode - operation code

//  INSTRUCTION SET ARCHITECTURE:
//  [OPC] INSTR ARGS        (OPERATION)
//  0x10  WRITE R1          (WRITE R1)
//  0x11  MOV   R1, R2      (R1 = R2)
//  0x12  MOVli R1, IMM     (R1 = IMM)
//  0x13  MOVhi R1, IMM     (R1 = IMM << 16)
//  0x21  ADD   R1, R2, R3  (R1 = R2 + R3)
//  0x31  ADDi  R1, R2, IMM (R1 = R2 + IMM)
//  0x22  SUB   R1, R2, R3  (R1 = R2 - R3)
//  0x32  SUBi  R1, R2, IMM (R1 = R2 - IMM)
//  0x23  MUL   R1, R2, R3  (R1 = R2 * R3)
//  0x33  MULi  R1, R2, IMM (R1 = R2 * IMM)
//  0x24  DIV   R1, R2, R3  (R1 = R2 / R3)
//  0x34  DIVi  R1, R2, IMM (R1 = R2 / IMM)

//  0x00  EXIT              (STOP EXECUTION)
//  0x70  B     IMM         (PC = PC + IMM)
//  0x71  B.EQ  R1, R2, IMM (if (R1 == R2) PC = PC + IMM)
//  0x72  B.NE  R1, R2, IMM (if (R1 != R2) PC = PC + IMM)
//  0x73  B.LE  R1, R2, IMM (if (R1 <= R2) PC = PC + IMM)
//  0x74  B.GT  R1, R2, IMM (if (R1 >  R2) PC = PC + IMM)

#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <cassert>
#include <iostream>
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <functional>

//#define DEBUG

using Reg_t = uint32_t;
using Imm_t = int16_t;
using InstrCode_t = uint32_t;

class State final {
  static constexpr auto RegFileSize = 16u;
  size_t PC = 0;
  std::vector<Reg_t> RegFile;

public:
  State() : RegFile(RegFileSize, 0u) {}

  void setPC(size_t NewPC) {
    PC = NewPC;
  }

  size_t getPC() const {
    return PC;
  }

  Reg_t &getReg(size_t N) {
    assert(N < RegFile.size());
    return RegFile[N];
  }
}; 

namespace decoder {
size_t getOpcode(InstrCode_t Instr) {
  constexpr auto OpcodeMask = 0xFF000000u;
  constexpr auto OpcodeOffset = 24u;
  return (Instr & OpcodeMask) >> OpcodeOffset;
}

size_t getR1(InstrCode_t Instr) {
  constexpr auto R1Mask = 0x00F00000u;
  constexpr auto R1Offset = 20u;
  return (Instr & R1Mask) >> R1Offset;
}

size_t getR2(InstrCode_t Instr) {
  constexpr auto R2Mask = 0x000F0000u;
  constexpr auto R2Offset = 16u;
  return (Instr & R2Mask) >> R2Offset;
}

size_t getR3(InstrCode_t Instr) {
  constexpr auto R3Mask = 0x0000FFFFu;
  return Instr & R3Mask;
}

Imm_t getImm(InstrCode_t Instr) {
  constexpr auto ImmMask = 0x0000FFFFu;
  return static_cast<Imm_t>(Instr & ImmMask);
}

std::string getName(InstrCode_t Instr) {
  static const auto OpcodeToName = 
    std::unordered_map<size_t, std::string>{
      {0x00, "EXIT"},
      {0x70, "B"},
      {0x71, "B.EQ"},
      {0x72, "B.NE"},
      {0x73, "B.LE"},
      {0x74, "B.GT"},
      {0x10, "WRITE"},
      {0x11, "MOV"},
      {0x12, "MOVli"},
      {0x13, "MOVhi"},
      {0x21, "ADD"},
      {0x31, "ADDi"},
      {0x22, "SUB"},
      {0x32, "SUBi"},
      {0x23, "MUL"},
      {0x33, "MULi"},
      {0x24, "DIV"},
      {0x34, "DIVi"}
    }; 
  auto Opcode = getOpcode(Instr);
  assert(OpcodeToName.find(Opcode) != OpcodeToName.end() 
         && "Unknown opcode");
  return OpcodeToName.find(Opcode)->second;
}

std::string getRegName(Reg_t Reg) {
  return "R" + std::to_string(Reg);
}

bool hasImm(InstrCode_t Instr) {
  static const auto HasImm =
    std::unordered_set<size_t>{
      0x12, 0x13, 0x31, 0x32, 0x33, 0x34,
      0x70, 0x71, 0x72, 0x73, 0x74
    };
  auto Opcode = getOpcode(Instr);
  return HasImm.find(Opcode) != HasImm.end();
}

bool hasR1(InstrCode_t Instr) {
  static const auto DoesntHaveR1 =
    std::unordered_set<size_t>{
      0x00, 0x70
    };
  auto Opcode = getOpcode(Instr);
  return DoesntHaveR1.find(Opcode) == DoesntHaveR1.end();
}

bool hasR2(InstrCode_t Instr) {
  static const auto DoesntHaveR2 =
    std::unordered_set<size_t>{
      0x00, 0x10, 0x12, 0x13, 0x70
    };
  auto Opcode = getOpcode(Instr);
  return DoesntHaveR2.find(Opcode) == DoesntHaveR2.end();
}

bool hasR3(InstrCode_t Instr) {
  static const auto HasR3 =
    std::unordered_set<size_t>{
      0x21, 0x22, 0x23, 0x24
    };
  auto Opcode = getOpcode(Instr);
  return HasR3.find(Opcode) != HasR3.end();
}

} // namespace decoder

namespace instructions {

} // namespace instructions

using InstExecution_t = std::function<State(InstrCode_t, State)>;

void execute(InstrCode_t Instr, State &CurState) {
  static const auto NameToInstr = 
    std::unordered_map<std::string, InstExecution_t> {
      {"WRITE", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          std::cout << CurState.getReg(R1) << std::endl;
          return CurState;
        }},
      {"MOV", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          CurState.getReg(R1) = CurState.getReg(R2);
          return CurState;
        }},
      {"MOVli", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          size_t Imm = decoder::getImm(Instr);
          CurState.getReg(R1) = Imm;
          return CurState;
        }},
      {"MOVhi", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          size_t Imm = decoder::getImm(Instr);
          CurState.getReg(R1) = Imm << 16;
          return CurState;
        }},
      {"ADD", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          auto R3 = decoder::getR3(Instr);
          CurState.getReg(R1) = CurState.getReg(R2) + CurState.getReg(R3);
          return CurState;
        }},
      {"ADDi", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          size_t Imm = decoder::getImm(Instr);
          CurState.getReg(R1) = CurState.getReg(R2) + Imm;
          return CurState;
        }},
      {"SUB", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          auto R3 = decoder::getR3(Instr);
          CurState.getReg(R1) = CurState.getReg(R2) - CurState.getReg(R3);
          return CurState;
        }},
      {"SUBi", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          size_t Imm = decoder::getImm(Instr);
          CurState.getReg(R1) = CurState.getReg(R2) - Imm;
          return CurState;
        }},
      {"MUL", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          auto R3 = decoder::getR3(Instr);
          CurState.getReg(R1) = CurState.getReg(R2) * CurState.getReg(R3);
          return CurState;
        }},
      {"MULi", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          size_t Imm = decoder::getImm(Instr);
          CurState.getReg(R1) = CurState.getReg(R2) * Imm;
          return CurState;
        }},
      {"DIV", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          auto R3 = decoder::getR3(Instr);
          CurState.getReg(R1) = CurState.getReg(R2) / CurState.getReg(R3);
          return CurState;
        }},
      {"DIVi", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          size_t Imm = decoder::getImm(Instr);
          CurState.getReg(R1) = CurState.getReg(R2) / Imm;
          return CurState;
        }},
      {"B", [](InstrCode_t Instr, State CurState) {
          auto Imm = decoder::getImm(Instr) / sizeof(InstrCode_t);
          size_t PrevPC = CurState.getPC() - 1;
          CurState.setPC(PrevPC + Imm);
          return CurState;
        }},
      {"B.EQ", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          auto Imm = decoder::getImm(Instr) / sizeof(InstrCode_t);
          size_t PrevPC = CurState.getPC() - 1;
          if (CurState.getReg(R1) == CurState.getReg(R2)) 
            CurState.setPC(PrevPC + Imm);
          return CurState;
        }},
      {"B.NE", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          auto Imm = decoder::getImm(Instr) / sizeof(InstrCode_t);
          size_t PrevPC = CurState.getPC() - 1;
          if (CurState.getReg(R1) != CurState.getReg(R2)) 
            CurState.setPC(PrevPC + Imm);
          return CurState;
        }},
      {"B.LE", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          auto Imm = decoder::getImm(Instr) / sizeof(InstrCode_t);
          size_t PrevPC = CurState.getPC() - 1;
          if (CurState.getReg(R1) <= CurState.getReg(R2)) 
            CurState.setPC(PrevPC + Imm);
          return CurState;
        }},
      {"B.GT", [](InstrCode_t Instr, State CurState) {
          auto R1 = decoder::getR1(Instr);
          auto R2 = decoder::getR2(Instr);
          auto Imm = decoder::getImm(Instr) / sizeof(InstrCode_t); 
          size_t PrevPC = CurState.getPC() - 1;
          if (CurState.getReg(R1) > CurState.getReg(R2)) 
            CurState.setPC(PrevPC + Imm);
          return CurState;
        }}
    };
  auto Name = decoder::getName(Instr);
  auto PC = CurState.getPC();
  #ifdef DEBUG
  std::cout << "-0x" << std::hex 
    << CurState.getPC() * sizeof(InstrCode_t) << ": " << Name << std::endl;
  #endif
  assert(NameToInstr.find(Name) != NameToInstr.end());
  //PC has number of the next instruction, not its address
  CurState.setPC(PC + 1);
  auto &&InstrExecution = NameToInstr.find(Name)->second;
  CurState = InstrExecution(Instr, std::move(CurState));
}

void printAsm(uint32_t * bin, uint32_t binSize) {
    auto Instructions = std::vector<InstrCode_t>(bin, bin + binSize);
    printf("INSTRUCTIONS:\n");
    for (auto Instr : Instructions) {
      std::cout << decoder::getName(Instr) << " ";
      if (decoder::hasR1(Instr))
        std::cout << decoder::getRegName(decoder::getR1(Instr)) << ", ";
      if (decoder::hasR2(Instr))
        std::cout << decoder::getRegName(decoder::getR2(Instr)) << ", ";
      if (decoder::hasR3(Instr))
        std::cout << decoder::getRegName(decoder::getR3(Instr)) << ", ";
      if (decoder::hasImm(Instr))
        std::cout << decoder::getImm(Instr);
      std::cout << std::endl;
    }
    printf("\n");
    return;
}

void execute(uint32_t * bin, uint32_t binSize) {
    auto Instructions = std::vector<InstrCode_t>(bin, bin + binSize);
    auto CurState = State{};
    printf("EXECUTION:\n");
    while (true) {
      auto PC = CurState.getPC();
      auto Instr = Instructions[PC];
      auto Name = decoder::getName(Instr);
      if (Name == "EXIT")
        break;
      execute(Instr, CurState);
    }
    printf("\n");
    return;
}

int main() {
    uint32_t BIN[] = {0x1200000A, 0x12100001, 0x12200001, 0x70000008,
                      0x00000000, 0x7301001C, 0x23220001, 0x7121000C,
                      0x10100000, 0x10200000, 0x31110001, 0x7000FFE8,
                      0x10200000, 0x7000FFDC};
    printAsm(BIN, sizeof(BIN)/sizeof(uint32_t));
    execute(BIN, sizeof(BIN)/sizeof(uint32_t));
    return 0;
}

// EXPECTED OUTPUT:
//
// INSTRUCTIONS:
// 0x0000 : MOVli R0, 10
// 0x0004 : MOVli R1, 1
// 0x0008 : MOVli R2, 1
// 0x000c : B 0x14
// 0x0010 : EXIT
// 0x0014 : B.LE R0, R1, 0x30
// 0x0018 : MUL R2, R2, R1
// 0x001c : B.EQ R2, R1, 0x28
// 0x0020 : WRITE R1
// 0x0024 : WRITE R2
// 0x0028 : ADDi R1, R1, 1
// 0x002c : B 0x14
// 0x0030 : WRITE R2
// 0x0034 : B 0x10
// 
// EXECUTION:
// 3
// 6
// 4
// 24
// 5
// 120
// 6
// 720
// 7
// 5040
// 8
// 40320
// 9
// 362880
// 362880