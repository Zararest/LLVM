#include "../include/Translator.h"

#include <algorithm>

namespace {

void removeComments(std::string &Program) {
  constexpr auto Comment = '#';
  constexpr auto CommentSubstitution = ' ';
  auto CommentBeg = std::find(Program.begin(), Program.end(), Comment);
  while (CommentBeg != Program.end()) {
    auto CommentEnd = std::find(std::next(CommentBeg), Program.end(), Comment);
    if (CommentEnd == Program.end())
      utils::reportFatalError("Wrong comments format");
    std::fill(CommentBeg, CommentEnd + 1, CommentSubstitution);
    CommentBeg = std::find(CommentEnd, Program.end(), Comment);
  }
}

template <typename T>
std::optional<translator::Token> tryTokenize(const std::string &Word) {
  if (T::isa(Word))
    return translator::Token{T{Word}};
  return std::nullopt;
}

translator::Token getToken(std::string Word) {
  auto Res = std::optional<translator::Token>{};
  if (Res = tryTokenize<translator::Token::Section>(Word))
    return *Res;
  if (Res = tryTokenize<translator::Token::Function>(Word))
    return *Res;
  if (Res = tryTokenize<translator::Token::Label>(Word))
    return *Res;
  if (Res = tryTokenize<translator::Token::Assign>(Word))
    return *Res;
  if (Res = tryTokenize<translator::Token::Word>(Word))
    return *Res;
  utils::reportFatalError("Unreachable");
  return *Res;
}

} // namespace

namespace translator {

std::vector<Token> tokenize(std::string Program) {
  removeComments(Program);
  std::replace(Program.begin(), Program.end(), ',', ' ');

  constexpr auto Delimiters = std::array{' ', '\n', '\t'};
  auto NotDelim = [Delimiters] (char C) { 
    return std::find(Delimiters.begin(), Delimiters.end(), C) == Delimiters.end(); 
  };
  auto WordBeg = std::find_if(Program.begin(), Program.end(), NotDelim);
  
  auto Res = std::vector<Token>{};
  while (WordBeg != Program.end()) {
    auto WordEnd = std::find_if_not(WordBeg, Program.end(), NotDelim);
    if (WordBeg != WordEnd)
      Res.emplace_back(getToken({WordBeg, WordEnd}));
    WordBeg = std::find_if(WordEnd, Program.end(), NotDelim);
  }

  return Res;
}

} // namespace translator