class SimReg<bits<4> Enc = 0, string n, int s> {
    bits<4> HWEncoding = Enc;
    string Name = n;
    string purpose;
    int Size = s;
}

class SimRegGPR<bits<4> Enc, string n, int s> : SimReg<Enc, n, s> {
  let purpose = "GPR";
}

class SimRegVEC<bits<4> Enc, string n, int s> : SimReg<Enc, n, s> {
  let purpose = "VEC";
}

def D1 : SimRegGPR<1, "d1", 64>;

foreach I = 2-4 in {
  def D#I : SimRegVEC<I, "d"#I, 64>;
}

foreach I = 1-4 in {
  def Q#I : SimRegVEC<I, "q"#I, 128>;
}
