#include <stdio.h>

int func(int x, int y) {
  int res = 10;
  if (x == y) {
    res += x;
  } else {
    res += y;
  }
  return x + res;
}

int bar(int x, int y) {
  int a = x + y;
  a = 1;
  return a;
}

int main() {
  for (int i = 0; i < 4; ++i)
    printf("%d\n", func(8, -5));
  bar(1, 1);
  return 0;
}