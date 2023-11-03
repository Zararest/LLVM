#pragma once

#include <iostream>

#include "Assembler.h"

namespace translator {

assembler::Code parse(std::istream &S);
void dump(assembler::Code &Code, std::ostream &S);

} // namespace translator