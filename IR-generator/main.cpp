#include "llvm/Support/CommandLine.h"

#include "Translator.h"

#include <sstream>
#include <fstream>

using namespace llvm;

cl::OptionCategory Assembler("Assembler");

static cl::opt<std::string> AsmFile("asm-file", 
                                    cl::desc("File with my assembler"),
                                    cl::cat(Assembler), cl::init(""));

static cl::opt<std::string> DumpTokens("dump-tokens",
                                cl::desc("File to dump tokens of the asm file"),
                                cl::cat(Assembler), cl::init(""));

static cl::opt<std::string> DumpMyIR("dump-parsed",
                              cl::desc("File to dump parsed program"),
                              cl::cat(Assembler), cl::init(""));

void dumpParsed(assembler::Code &Code) {
  auto File = std::ofstream{DumpMyIR};
  if (!File.is_open())
    utils::reportFatalError("Can't open file to dump parsed");
  
  Code.dump(File);
}

template <typename It>
void dumpTokens(It Beg, It End) {
  auto File = std::ofstream{DumpTokens};
  if (!File.is_open())
    utils::reportFatalError("Can't open file to dump tokens");
  translator::dumpTokens(Beg, End, File);
}

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
  
  if (!DumpTokens.empty())
    dumpTokens(Tokens.begin(), Tokens.end());
  
  auto Code = translator::parse(Tokens);
  
  if (!DumpMyIR.empty())
    dumpParsed(Code);
}