#include <stdint.h>
#include <stdio.h>

static unsigned RecursionDepth;

void logFuncStart(const char *FuncName) {
  for (unsigned i = 0; i < RecursionDepth; ++i)
    printf("  ");
  printf("In func: %s\n", FuncName);
  RecursionDepth++;
}

void logFuncEnd(const char *FuncName) {
  for (unsigned i = 0; i < RecursionDepth; ++i)
    printf("  ");
  printf("Return from func: %s\n", FuncName);
  RecursionDepth--;
}

void logBinOp(int Op, int Lhs, int Rhs, const char *OpName, long long InstrId) {
  for (unsigned i = 0; i < RecursionDepth; ++i)
    printf("  ");
  printf("%s[%lli] %i = %i %i\n", OpName, InstrId, Op, Lhs, Rhs);
}
 
void logCmpOp(int Predicate, const char *OpName, long long InstrId) {
  for (unsigned i = 0; i < RecursionDepth; ++i)
    printf("  ");
  printf("%s[%lli] predicate: %i\n", OpName, InstrId, Predicate);
}

void logCallOp(const char *FuncName) {
  for (unsigned i = 0; i < RecursionDepth; ++i)
    printf("  ");
  printf("Calling func %s\n", FuncName);
}