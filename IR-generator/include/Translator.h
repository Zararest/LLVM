#pragma once

#include <iostream>
#include <cassert>

#include "Assembler.h"
#include "Utils.h"

namespace translator {

struct Token {
  struct Section {
    std::string Name;

    Section(const std::string &Word) {
      assert(isa(Word));
      Name = std::string{std::next(Word.begin()), Word.end()};
    } 

    static bool isa(const std::string &Word) {
      return Word.front() == '.';
    }

    void dump(std::ostream &S) {
      S << '.' << Name << std::endl;
    }
  }; 

  struct Function {
    std::string Name;

    Function(const std::string &Word) {
      assert(isa(Word));
      Name = std::string{std::next(Word.begin()), std::prev(Word.end())};
    }

    static bool isa(const std::string &Word) {;
      return Word.front() == '<' && Word.back() == '>';
    }

    void dump(std::ostream &S) {
      S << '<' << Name << '>' << std::endl;
    }
  }; 

  struct Label {
    std::string Name;
    
    Label(const std::string &Word) {
      assert(isa(Word));
      Name = std::string{Word.begin(), std::prev(Word.end())};
    }

    static bool isa(const std::string &Word) {
      return Word.back() == ':';
    }

    void dump(std::ostream &S) {
      S << Name << ':' << std::endl;
    }
  }; 

  struct Assign {
    Assign(const std::string &Word) {}

    static bool isa(const std::string &Word) {
      return Word == "->";
    }

    void dump(std::ostream &S) {
      S << " -> " << std::endl;
    }
  }; 
  
  // Должен проверяться послежним
  struct Word {
    std::string Name;

    static bool isa(const std::string &Word) {
      return true;
    }

    void dump(std::ostream &S) {
      S << Name << ' ' << std::endl;
    }
  }; 

  std::string getName() {
    if (std::holds_alternative<Assign>(Value))
      utils::reportFatalError("Assign token doesn't have a name");
    return std::visit([](auto &&Arg) { return Arg.Name; });
  }

  std::variant<Section, Function, Label, Assign, Word> Value;
};

std::vector<Token> tokenize(std::string Program);

template <typename It>
void dumpTokens(It Beg, It End, std::ostream &S) {
  for (auto &Token : utils::makeRange(Beg, End))
    std::visit([&S](auto &&Arg) { Arg.dump(S); }, Token.Value);
  S << std::endl;
}

assembler::Code parse(std::vector<Token> Program);
void dump(assembler::Code &Code, std::ostream &S);

} // namespace translator