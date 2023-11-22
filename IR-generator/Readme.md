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
Для данной программы все поля Dot занимают 64 бита.

## Структура программы
Программа разделена на секции:
```
#комментарии вырезаются из программы#

.global 
...глобальные переменные и их значения...
start *название входной функции*

.text
...функции вида <название функции>...
BB: - базовый блок в функции
```

## Вызов функций и регистры
Возможные регистры:
```
t0-11 - регистры локальные для каждой функции
a0-3 - аргументы функции
r0 - возвращаемое значение
```

Для каждой функции t регистры уникальные. Изменение `t` регситров в одной функции не влияет на `t` регистры в другой.

Регистры `a`, `r` одни для всех функций.

Функция может как угодно изменять `a`, `r` регистры, поэтому перед вызовом нужно делать спилы.

## Модель памяти
Структуры хранятся отдельно от регистров и доступ к ним производится по указателям. Нулевой указатель - аналог nullptr.

## Инструкции

Интсрукции, изменяющие регистры имеют обозначение `instr args -> val`.
При ее исполнении сначала считываются все значения и только потом результат записывается в регистр.

Особые инструкции:
```
Общая нотация: 
[название]:(Возвращаемое значение если есть) (значение инструкции) (аргументы)+


[incJump]: (Val) (add + icmp + br) (RegToInc) (ImmToCmp) (Label1) (Label2)
  Val = add i64 RegToInc, 1
  Cond = icmp eq i64 Val, ImmToCmp
  br i1 Cond, Label1, Label2

[loadDotField]: (Val) (gep + load) (StructPtr) (ImmOff)
  Addr = getelementptr inbounds %Dot, %Dot* StructPtr, i64 0, i32 ImmOff
  Val = load i64, i64* Addr, align 8

[storeDotFiled]: (gep + store) (StructPtr) (ImmOff) (ValToStore)
  Ptr = getelementptr inbounds %Dot, %Dot* StructPtr, i64 0, i32 ImmOff
  store i64 ValToStore, i64* Ptr, align 8

[storeDotFiledImm]: (gep + store) (StructPtr) (ImmOff) (ImmToStore)
  Ptr = getelementptr inbounds %Dot, %Dot* StructPtr, i64 0, i32 ImmOff
  store i64 ImmToStore, i64* Ptr, align 8

[jumpIfDot]: (gep + icmp + br) (StructPtr) (ImmOff) (ValCmpWith) (Label1) (Label2)
  Ptr = getelementptr inbounds %struct.Dot, %struct.Dot* StructPtr, i64 0, i32 ImmOff
  Field = load i64, i64* Ptr, align 8
  Cond = icmp ugt i64 ValCmpWith, Filed
  br i1 Cond, label Label1, label Label2

[getDotAddr]: (Addr) (gep) (StructPtr) (ValOff)
  Addr = getelementptr inbounds %struct.Dot, %struct.Dot* StructDot, i64 ValOff

[initRgb]: (...) (StructPtr) (Seed)
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

[loadRgb]: (Val) (gep + bitcast + load) (StructPtr)
  Addr = getelementptr inbounds %Dot, %Dot* StructPtr, i64 0, i32 5
  Bitcast = bitcast %struct.RGB* Addr to i24*
  Val = load i24, i24* Bitcast, align 8

[cmpTwo]: (Val) (icmp + icmp + and) (ValCmpWith) (Val1) (Val2)
  Cmp1 = icmp ult i64 ValCmpWith, Val1
  Cmp2 = icmp ult i64 ValCmpWith, Val2
  Val = and i1 Cmp1, Cmp2

[norm]: (norm) (mul + mul + add) (x1) (x2)
  x1Square = mul i64 x1, x1
  x2Square = mul i64 x2, x2
  norm = add i64 x1Square, x2Square 

[xorshift]: (Val) (xorshift) (Seed)
  %1 = load i64, i64* Seed, align 8, !tbaa !2
  %2 = shl i64 %1, 13
  %3 = xor i64 %2, %1
  %4 = lshr i64 %3, 7
  %5 = xor i64 %4, %3
  %6 = shl i64 %5, 17
  %7 = xor i64 %6, %5
  store i64 %7, i64* Seed, align 8, !tbaa !2
  ret i64 %7

[createDots]:(Ptr) (...) (ImmSize)
  %0 = alloca [ImmSize x %Dot], align 8
  %1 = bitcast [ImmSize x %Dot]* %0 to i8*
  Ptr = getelementptr inbounds [ImmSize x %Dot], [ImmSize x %Dot]* %0, i64 0, i64 0
```

Обычные инструкции:
```
[and]:(Val) (and) (ValIn) (Imm)
  Val = and i64 ValIn, Imm

[ret]:(ret)
  ret void

[cmpEqImm]:(Val) (icmp) (ValIn) (Imm)
  Val = icmp eq i64 ValIn, Imm 

[brCond]:(br) (Cond) (Label1) (Label2)
  br i1 Cond, label Label1, label Label2

[br]:(br) (Label)
  br label Label

[call]:(call) (FuncName)
  call FuncName(...) #аргументы будут переданы через регистры#

[cmpUGTImm]:(Val) (icmp) (ValIn) (Imm)
  Val = icmp ugt i64 ValIn, Imm

[li]:(Val) (li) (Imm)
  Val = Imm

[mul]:(Val) (mul) (Lhs) (Rhs)
  Val = mal Lhs, Rhs

[add]:(Val) (add) (Lhs) (Rhs)
  Val = add Lhs, Rhs

[select]: (Val) (select) (Cond) (ValIn1) (ValIn2)
  Val = select i1 Cond, i64 ValIn1, i64 ValIn2

[mv]: (Val) (mv) (Reg)
  Val = Reg
```