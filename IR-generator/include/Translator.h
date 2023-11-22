#pragma once

#include <iostream>
#include <cassert>

#include "Assembler.h"

namespace translator {

struct Token {
  struct Section {
    std::string Name;

    Section(const std::string &Word) {
      assert(isa(Word));
      Name = std::string{std::next(Word.begin()), Word.end()};
    } 

    static bool isa(const std::string &Word) {
      assert(Word.size() >= 2);
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

    static bool isa(const std::string &Word) {
      assert(Word.size() >= 3);
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
      assert(Word.size() >= 2);
      return Word.back() == ':';
    }

    void dump(std::ostream &S) {
      S << Name << ':' << std::endl;
    }
  }; 

  struct Assign {
    Assign(const std::string &Word) {}

    static bool isa(const std::string &Word) {
      assert(Word.size());
      return Word == "->";
    }

    void dump(std::ostream &S) {
      S << " -> ";
    }
  }; 
  
  // Должен проверяться послежним
  struct Word {
    std::string Name;

    static bool isa(const std::string &Word) {
      assert(Word.size());
      return true;
    }

    void dump(std::ostream &S) {
      S << Name << ' ';
    }
  }; 

  std::variant<Section, Function, Label, Assign, Word> Value;
};

std::vector<Token> tokenize(std::string Program);

assembler::Code parse(std::vector<Token> Program);
void dump(assembler::Code &Code, std::ostream &S);

} // namespace translator