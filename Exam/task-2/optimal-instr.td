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