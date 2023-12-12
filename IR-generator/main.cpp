#include "llvm/Support/CommandLine.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

#include "Translator.h"
#include "Graphics.h"

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

static cl::opt<bool> GeneratePseudoIR("generate-pseudo-ir",
                              cl::desc("Generates IR with external calls"),
                              cl::cat(Assembler), cl::init(false));

static cl::opt<bool> GenerateRealIR("generate-real-ir",
                              cl::desc("Generates IR without external functions"),
                              cl::cat(Assembler), cl::init(false));

void dumpParsed(assembler::Code &Code) {
  auto File = std::ofstream{DumpMyIR};
  if (!File.is_open())
    utils::reportFatalError("Can't open file to dump parsed");
  
  Code.dump(File);
}

void execute(translator::IRToExecute IRWithEnv) {
  std::cout << "==Executing code==" << std::endl;
  IRWithEnv.IR.M->print(outs(), nullptr);

  std::cout << "==Running==" << std::endl;
  InitializeNativeTarget();
  InitializeNativeTargetAsmPrinter();
  auto *EE = EngineBuilder(std::move(IRWithEnv.IR.M)).create();
  EE->InstallLazyFunctionCreator(IRWithEnv.FuncMapper);
  // Lazy mapper can also handle globals
  EE->finalizeObject();

  auto Args = std::vector<GenericValue>{};

  lib::simInit();
  EE->runFunction(IRWithEnv.StartFunc, Args);
  lib::simExit();
  std::cout << "==End==" << std::endl;
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

  if (GenerateRealIR)
    execute(translator::makeLLVMIR(Code));

  if (GeneratePseudoIR)
    execute(translator::makePseudoLLVMIR(Code));
}