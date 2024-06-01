# Target code
![img](./Target-code.PNG)

## Solution
## Non-optimal
Table gen file:
```
class Instr<int opc, string n, dag ops> {
  int Opcode = opc;
  string AsmString = !strconcat(n, " $dst, $src1, $src2");
  dag Operands = ops;
}

def GPR;
def Imm;
def ops;

def ADD_ri : Instr<7, "add", (ops GPR:$dst, GPR:$src1, Imm:$src2)>;
def ADD_rr : Instr<7, "add", (ops GPR:$dst, GPR:$src1, GPR:$src2)>;

def MUL_ri : Instr<4, "mul", (ops GPR:$dst, GPR:$src1, Imm:$src2)>;
def MUL_rr : Instr<4, "mul", (ops GPR:$dst, GPR:$src1, GPR:$src2)>;

def SUB_ri : Instr<5, "mul", (ops GPR:$dst, GPR:$src1, Imm:$src2)>;
def SUB_rr : Instr<5, "mul", (ops GPR:$dst, GPR:$src1, GPR:$src2)>;
```

## Optimal
Solution with multiclass"
```
class Instr<int opc, string n, dag ops> {
  int Opcode = opc;
  string AsmString = !strconcat(n, " $dst, $src1, $src2");
  dag Operands = ops;
}

def GPR;
def Imm;
def ops;

multiclass Pair<int opc, string n> {
  def "_ri" : Instr<opc, n, (ops GPR:$dst, GPR:$src1, Imm:$src2)>;
  def "_rr" : Instr<opc, n, (ops GPR:$dst, GPR:$src1, GPR:$src2)>;
}

defm "ADD" : Pair<7, "add">;
defm "MUL" : Pair<4, "mul">;
defm "SUB" : Pair<5, "sub">;
```

Command to run:
```bash
llvm-tbgen optimal-instr.td
```