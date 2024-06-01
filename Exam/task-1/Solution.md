# Initial code
This code should be translated to the LLVM IR with memory and with PHI nodes:
```c
int smax(int *arr, unsigned int size) {
  if (size == 0)
    return 0;
  int max = arr[0];
  for (unsigned int i = 0; i < size; i++) 
    if (arr[i] > max)
      max = arr[i];
  
  return max;
}
```

## Memory SSA
In order to generate file with memory you should run:
```bash
clang -emit-llvm -S initial.c -o memory-ssa.ll
```

Answer:
```
define dso_local i32 @smax(ptr noundef %0, i32 noundef %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca ptr, align 8
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store ptr %0, ptr %4, align 8
  store i32 %1, ptr %5, align 4
  %8 = load i32, ptr %5, align 4
  %9 = icmp eq i32 %8, 0
  br i1 %9, label %10, label %11

10:                                               ; preds = %2
  store i32 0, ptr %3, align 4
  br label %39

11:                                               ; preds = %2
  %12 = load ptr, ptr %4, align 8
  %13 = getelementptr inbounds i32, ptr %12, i64 0
  %14 = load i32, ptr %13, align 4
  store i32 %14, ptr %6, align 4
  store i32 0, ptr %7, align 4
  br label %15

15:                                               ; preds = %34, %11
  %16 = load i32, ptr %7, align 4
  %17 = load i32, ptr %5, align 4
  %18 = icmp ult i32 %16, %17
  br i1 %18, label %19, label %37

19:                                               ; preds = %15
  %20 = load ptr, ptr %4, align 8
  %21 = load i32, ptr %7, align 4
  %22 = zext i32 %21 to i64
  %23 = getelementptr inbounds i32, ptr %20, i64 %22
  %24 = load i32, ptr %23, align 4
  %25 = load i32, ptr %6, align 4
  %26 = icmp sgt i32 %24, %25
  br i1 %26, label %27, label %33

27:                                               ; preds = %19
  %28 = load ptr, ptr %4, align 8
  %29 = load i32, ptr %7, align 4
  %30 = zext i32 %29 to i64
  %31 = getelementptr inbounds i32, ptr %28, i64 %30
  %32 = load i32, ptr %31, align 4
  store i32 %32, ptr %6, align 4
  br label %33

33:                                               ; preds = %27, %19
  br label %34

34:                                               ; preds = %33
  %35 = load i32, ptr %7, align 4
  %36 = add i32 %35, 1
  store i32 %36, ptr %7, align 4
  br label %15, !llvm.loop !6

37:                                               ; preds = %15
  %38 = load i32, ptr %6, align 4
  store i32 %38, ptr %3, align 4
  br label %39

39:                                               ; preds = %37, %10
  %40 = load i32, ptr %3, align 4
  ret i32 %40
}
```

## Phi nodes
Run:
```bash
clang -emit-llvm -S -O1 initial.c -o ssa.ll
```

Answer:
```
define dso_local i32 @smax(ptr nocapture noundef readonly %0, i32 noundef %1) local_unnamed_addr #0 {
  %3 = icmp eq i32 %1, 0
  br i1 %3, label %15, label %4

4:                                                ; preds = %2
  %5 = load i32, ptr %0, align 4, !tbaa !5
  %6 = zext i32 %1 to i64
  br label %7

7:                                                ; preds = %4, %7
  %8 = phi i64 [ 0, %4 ], [ %13, %7 ]
  %9 = phi i32 [ %5, %4 ], [ %12, %7 ]
  %10 = getelementptr inbounds i32, ptr %0, i64 %8
  %11 = load i32, ptr %10, align 4, !tbaa !5
  %12 = tail call i32 @llvm.smax.i32(i32 %11, i32 %9)
  %13 = add nuw nsw i64 %8, 1
  %14 = icmp eq i64 %13, %6
  br i1 %14, label %15, label %7, !llvm.loop !9

15:                                               ; preds = %7, %2
  %16 = phi i32 [ 0, %2 ], [ %12, %7 ]
  ret i32 %16
}
```