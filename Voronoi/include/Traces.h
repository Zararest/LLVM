#pragma once

#include "llvm/ADT/iterator_range.h"

#include "Utils.h"

#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <optional>
#include <numeric>
#include <utility>
#include <set>
#include <algorithm>
#include <iostream>

struct Instr {
  size_t Id;
  std::string Name;

  Instr(size_t Id) : Id{Id} {}
  Instr(size_t Id, const std::string &Name) : Id{Id}, Name{Name} {}

  bool isCFInstr() const {
    return Name == "call" || Name == "ret" ||
           Name == "phi"  || Name == "br";
  }

  bool operator <(const Instr &Rhs) const {
    if (Id != Rhs.Id)
      return Id < Rhs.Id;
    return Name < Rhs.Name;
  }
};

class DynamicTrace {
  std::vector<Instr> Trace;

public:
  DynamicTrace(const std::string &TraceFile) {
    auto S = std::ifstream{TraceFile};
    if (!S.is_open())
      utils::reportFatalError("Can't open dyn trace file");
    
    auto InstrId = 0ull;
    while (S >> InstrId)
      Trace.emplace_back(InstrId);
  }

  auto begin() {
    return Trace.begin();
  }

  auto end() {
    return Trace.end();
  }
};

class StaticTrace {
  std::vector<Instr> Trace;
  std::unordered_map<size_t, std::string> InstrIdToName;

  std::vector<Instr>::iterator CachedIt;

public:
  StaticTrace(const std::string &TraceFile) {
    auto S = std::ifstream{TraceFile};
    if (!S.is_open())
      utils::reportFatalError("Can't open static trace file");

    auto InstrId = 0ull;
    while (S >> InstrId) {
      auto Name = std::string{};
      if (!(S >> Name))
        utils::reportFatalError("No name on instr: " + std::to_string(InstrId));
      Trace.emplace_back(InstrId, Name);
      InstrIdToName[InstrId] = Name;
    }
    CachedIt = Trace.begin();
  }

  StaticTrace(StaticTrace&&) = delete;

  std::optional<std::string> getName(size_t InstrId) {
    if (InstrIdToName.find(InstrId) != InstrIdToName.end())
      return InstrIdToName.find(InstrId)->second;
    return std::nullopt;
  }

  auto find(size_t InstrId) {
    auto ResIt = std::find_if(CachedIt, Trace.end(), 
                              [InstrId](const Instr &I) {
                                  return I.Id == InstrId;
                              });
    if (ResIt != Trace.end()) {
      CachedIt = ResIt;
      return ResIt;
    }

    ResIt = std::find_if(Trace.begin(), Trace.end(), 
                        [InstrId](const Instr &I) {
                            return I.Id == InstrId;
                        });
    CachedIt = ResIt;
    return ResIt;
  }

  auto begin() {
    return Trace.begin();
  }

  auto end() {
    return Trace.end();
  }
};

struct InstrBundle {
  std::vector<Instr> Bundle;

  bool operator <(const InstrBundle &Rhs) const {
    auto &LBundle = Bundle;
    auto &RBundle = Rhs.Bundle;  
    if (LBundle.size() != RBundle.size())
      return LBundle.size() < RBundle.size();
    for (size_t i = 0; i < LBundle.size(); ++i)
      if ((LBundle[i] < RBundle[i]) || 
          (RBundle[i] < LBundle[i]))
        return LBundle[i] < RBundle[i];
    return false;
  }

  bool operator ==(const InstrBundle &Rhs) const {
    return !(*this < Rhs) && !(Rhs < *this);
  }

  struct Hash {
    size_t operator ()(const InstrBundle &Bundle) const {
      auto Ids = std::vector<size_t>{};
      for (auto &[Id, Name] : Bundle.Bundle)
        Ids.push_back(Id);
      return std::accumulate(Ids.begin(), Ids.end(), 
                             0ull, std::multiplies<size_t>());
    }
  };
};

struct Histogram {
  struct BundleData {
    InstrBundle IBundle;
    size_t Count;

    BundleData(const InstrBundle &Bundle, size_t Count) : IBundle{Bundle}, 
                                                          Count{Count} {}

    bool operator <(const BundleData &Rhs) const {
      if (Count != Rhs.Count) 
        return Count < Rhs.Count;
      return IBundle < Rhs.IBundle;
    }
  };

  std::set<BundleData> Distribution;

  template <typename It>
  Histogram(It MapBeg, It MapEnd) {
    for (auto &[Bundle, Count] : llvm::make_range(MapBeg, MapEnd))
      Distribution.emplace(Bundle, Count);
  }

  Histogram() = default;

  class Builder {
    StaticTrace &StaticTr;
    DynamicTrace &DynTr;

    std::string getNameFromStatTrace(const Instr &I) {
      auto StatInstrIt = StaticTr.find(I.Id);
      if (StatInstrIt == StaticTr.end())
        return "Unknown";
      return StatInstrIt->Name;
    }

    template <typename It>
    std::optional<InstrBundle> getBundle(size_t Size, It DynIt) {
      if (DynTr.end() - DynIt < Size)
        return std::nullopt;
      auto BundleEnd = DynIt + Size;
      auto CFIt = std::find_if(DynIt, BundleEnd,
                                [&](Instr I) {
                                  I.Name = getNameFromStatTrace(I);
                                  return I.isCFInstr();
                                });
      if (CFIt < std::prev(BundleEnd))
        return std::nullopt;

      auto IBundle = InstrBundle{};
      std::transform(DynIt, BundleEnd, std::back_inserter(IBundle.Bundle),
                     [&](const Instr &I) {
                       return Instr{0, getNameFromStatTrace(I)};
                     });
      return IBundle;
    }

    using MapBundleCount = std::unordered_map<InstrBundle, size_t, 
                                              InstrBundle::Hash>;
    
    void increaseBundleCount(InstrBundle Bundle, MapBundleCount &Map) {
      if (Map.find(Bundle) != Map.end()) {
        auto It = Map.find(Bundle);
        It->second++;
        return; 
      }
      Map.emplace(Bundle, 1ull);
    } 

  public:
    Builder(StaticTrace &StaticTr, DynamicTrace &DynTr) : StaticTr{StaticTr},
                                                        DynTr{DynTr} {}

    Histogram getHistogram(size_t BundleSize) {
      auto BundleToCount = MapBundleCount{};
      if (std::distance(DynTr.begin(), DynTr.end()) < BundleSize)
        return Histogram{};

      auto End = DynTr.end() - BundleSize;
      for (auto DynIt = DynTr.begin(); DynIt != End; ++DynIt) {
        auto BundleOpt = getBundle(BundleSize, DynIt);
        if (BundleOpt)
          increaseBundleCount(*BundleOpt, BundleToCount);
      }
      
      return {BundleToCount.begin(), BundleToCount.end()};
    }
  };
};
