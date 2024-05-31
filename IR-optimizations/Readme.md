# IR optimizations

Task: optimize LLVM IR

Types of optimizations:
- translating from memory SSA to true SSA
- performance optimizations

## Tools
LLVM IR generation:
```bash
clang -emit-llvm -S example.c
```

Dump IR after each pass:
```bash
clang -mllvm -print-after-all -c example.c 
```

Dump all pass names:
```bash
clang -mllvm -debug-pass=Structure -c example.c 
```

Run one pass:
```bash
opt -passes='mem2reg,dce' -S dce.ll
```

## Memory SSA translation
First of all compiler creates `memory` SSA form.
It is a form where there are only `aloca` and `store` instead of variables.
This approach is not efficient enougth, because a lot of variables could be placed in registers instead of memory,
so you can enable clang to create true SSA form of a program:

Memory SSA:
```bash
clang -emit-llvm -S example.c
```

```
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 0, ptr %2, align 4
  ...
```

True SSA:
```bash
clang -emit-llvm -O1 -S example.c
```

```
define dso_local i32 @main() local_unnamed_addr #1 {
  br label %2

1:                                                ; preds = %2
  ret i32 0

2:                                                ; preds = %0, %2
  %3 = phi i32 [ 0, %0 ], [ %5, %2 ]
  %4 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @.str, i32 noundef 13)
  %5 = add nuw nsw i32 %3, 1
  %6 = icmp eq i32 %5, 4
  br i1 %6, label %1, label %2, !llvm.loop !5
}
```

## Performance optimizations
There are a lot of passes that transforms code in a way it executes faster.
For example:
- dead code elimination: 
```bash
opt -passes='mem2reg,dce' -S dce.ll
```
Remove code that won't be executed.
- loop unroll:
```bash
opt -passes='loop-unroll' -S loop-unroll.ll
```
