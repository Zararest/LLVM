# Ручная генерация IR
В файле `handmade-IR` находится llvm-IR сгенерированный руками и результат исполнения на симуляторе.

## Сборка
```
mkdir build
cd build
cmake ..
make

./Voronoi-IR
```

# ISA

!!! теперь все поля в Dot i64 !!!

Особые инструкции:
```
[incJump]: (add + icmp + br) (RegToInc) (ImmToCmp) (Label1) (Label2)
  Tmp = add i64 RegToInc, 1
  Cond = icmp eq i64 Tmp, ImmToCmp
  br i1 Cond, Label1, Label2

[loadDotField]: (Val) (gep + load) (StructPtr) (FieldOffs)
  Addr = getelementptr inbounds %Dot, %Dot* StructPtr, i64 0, i32 FieldOffs
  Val = load i64, i64* Addr, align 8

[storeDotFiled]: (gep + store) (StructPtr) (FiledOffs) (ValToStore)
  Ptr = getelementptr inbounds %Dot, %Dot* StructPtr, i64 0, i32 FiledOffs
  store i64 ValToStore, i64* Ptr, align 8

[initRgb]: (...) (StructPtr)
  %10 = call i64 @xorshift()
  %11 = trunc i64 %10 to i8
  %12 = getelementptr inbounds %Dot, %Dot* StructPtr, i64 0, i32 5, i32 0
  store i8 %11, i8* %12, align 8
  %13 = call i64 @xorshift()
  %14 = trunc i64 %13 to i8
  %15 = getelementptr inbounds %Dot, %Dot* StructPtr, i64 0, i32 5, i32 1
  store i8 %14, i8* %15, align 8
  %16 = call i64 @xorshift()
  %17 = trunc i64 %16 to i8
  %18 = getelementptr inbounds %Dot, %Dot* StructPtr, i64 0, i32 5, i32 2
  store i8 %17, i8* %18, align 8

[loadRgb]: (Val) (gep + bitcast + )

[cmpTwo]: (Val) (icmp + icmp + and) (ValCmpWith) (Val1) (Val2)
  Cmp1 = icmp ult i64 ValCmpWith, Val1
  Cmp2 = icmp ult i64 ValCmpWith, Val2
  Val = and i1 Cmp1, Cmp2

[norm]: (norm) (mul + mul + add) (x1) (x2)
  x1Square = mul i64 x1, x1
  x2Square = mul i64 x2, x2
  norm = add i64 x1Square, x2Square 

```

Обычные инструкции:
```

```