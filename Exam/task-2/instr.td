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