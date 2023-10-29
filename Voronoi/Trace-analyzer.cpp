#include "llvm/Support/CommandLine.h"

#include "Utils.h"
#include "Traces.h"

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace llvm;

cl::OptionCategory Analyzer("Trace analyzer");

static cl::opt<size_t> BundleSize("bundle-size",
                                  cl::desc("Size of the bundle of instructions "
                                           "to analyze"),
                                  cl::cat(Analyzer), cl::init(0));

static cl::opt<bool> PrintAllInstr("print-all-instr",
                                  cl::desc("Prints all instructions "
                                           "from stat trace"),
                                  cl::cat(Analyzer), cl::init(false));

static cl::opt<std::string> StaticTraceFile("static-trace",
                                    cl::desc("File with static trace"),
                                    cl::cat(Analyzer), cl::init(""));                                        

static cl::opt<std::string> DynamicTraceFile("dynamic-trace",
                                    cl::desc("File with dynamic trace"),
                                    cl::cat(Analyzer), cl::init(""));


int main(int Argc, char **Argv) {
  cl::ParseCommandLineOptions(Argc, Argv);

  if (DynamicTraceFile.empty())
    utils::reportFatalError("Dynamic trace hasn't been specified");
  
  if (StaticTraceFile.empty())
    utils::reportFatalError("Static trace hasn't been specified");

  auto StatTrace = StaticTrace{StaticTraceFile};
  auto DynTrace = DynamicTrace{DynamicTraceFile};
  auto HistBuilder = Histogram::Builder{StatTrace, DynTrace};
  auto Hist = HistBuilder.getHistogram(BundleSize);

  if (PrintAllInstr) {
    auto Names = std::set<std::string>{};
    std::transform(StatTrace.begin(), StatTrace.end(),
                   std::inserter(Names, Names.begin()),
                   [](const Instr &I) {
                    return I.Name;
                   });
    std::cout << "All instructions:" << std::endl; 
    for (auto Name : Names)
      std::cout << Name << std::endl;
  }

  if (BundleSize != 0) {
    std::cout << "Hist size: " << Hist.Distribution.size() << std::endl;
    for (auto &[Bundle, Count] : llvm::make_range(Hist.Distribution.rbegin(),
                                                  Hist.Distribution.rend())) {
      for (auto &I : Bundle.Bundle)
        std::cout << std::setw(13) << I.Name << " ";
      std::cout << std::setw(0) << ": " << Count << std::endl;
    }
  }
}