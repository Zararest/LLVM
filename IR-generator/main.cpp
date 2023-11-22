#include "llvm/Support/CommandLine.h"

#include "Translator.h"

using namespace llvm;

cl::OptionCategory Assembler("Assembler");

static cl::opt<std::string> AsmFile("asm-file", 
                                    cl::desc("File with my assembler"),
                                    cl::cat(Assembler), cl::init(""));

int main(int Argc, char **Argv) {
  cl::ParseCommandLineOptions(Argc, Argv);
}