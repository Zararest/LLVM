#include "llvm/Support/CommandLine.h"

#include "Translator.h"

#include <sstream>
#include <fstream>

using namespace llvm;

cl::OptionCategory Assembler("Assembler");

static cl::opt<std::string> AsmFile("asm-file", 
                                    cl::desc("File with my assembler"),
                                    cl::cat(Assembler), cl::init(""));

static cl::opt<bool> DumpTokens("dump-tokens",
                                cl::desc("Dumping tokens of the asm file"),
                                cl::cat(Assembler), cl::init(false));

int main(int Argc, char **Argv) {
  cl::ParseCommandLineOptions(Argc, Argv);

  if (AsmFile.empty())
    utils::reportFatalError("Asm file hasn't been specified");
  
  auto SS = std::stringstream{};
  auto File = std::ifstream{AsmFile};
  if (!File.is_open())
    utils::reportFatalError("Can't open asm file");

  SS << File.rdbuf();
  
  auto Asm = std::string{SS.str()};
  auto Tokens = translator::tokenize(Asm);
  std::cout << "Tokens number: " << Tokens.size() << std::endl;
  
  if (DumpTokens)
    translator::dumpTokens(Tokens.begin(), Tokens.end(), std::cout);
}