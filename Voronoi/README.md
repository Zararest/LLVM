# Описание
Данная программа реализует динамическую диаграму Вороного. 

Пример:
![img](img/Voronoi-diag.png)

Количество точек и скорость роста задаются через `define`.

## Запуск
Должна быть установлена бибилотека SDL

Сборка:
```
mkdir build
cd build
cmake ..
make
```

Запуск программы:
```
./Voronoi
```

## Получение llvm IR
```
make install
```

В файле `Engine-IR.ll` лежит IR для файла Engine.c, скомпилированного на `-O1`.

## Анализ трассы
### Получение трассы
При компиляции проекта на `dbgs()` подается статическая трасса программы.
При исполнении программы на стандартный выход подается динамическая трасса.

### Анализатор
Для анализа трассы есть отдельный target:
```
make Analyzer

./Analyzer --help
```

Для работы анализатора необхадимы 2 файла: один со статической трассой, другой с динамической.

### Результаты анализа
Размер окна приложения и количество кадров было уменьшено, для получения трассы адекватных размеров.
Трассы лежат в папке trace.

Все инструкции, используемые в программе:
```
add
alloca
and
bitcast
br
call
getelementptr
icmp
load
lshr
mul
phi
ret
select
sext
shl
store
sub
trunc
xor
```

Гистограмма этих интсрукций:
```
         call : 590092
         icmp : 213086
getelementptr : 164562
          sub : 163840
       select : 163840
         load : 123512
          mul : 122890
          add : 86100
          and : 40980
        trunc : 4190
      bitcast : 602
          xor : 150
          shl : 100
        store : 91
         lshr : 50
         sext : 10
          ret : 2
       alloca : 1
```

Гистограмма со связками инструкций учитвает control-flow инструкции.
Это значит, что если между двумя инструкциями в статической трассе есть control-flow инструкция, то в динамической трассе они не могут быть в одной связке.

Для двух инструкций:
```
getelementptr          load : 122910
       select          call : 122880
          add          call : 86090
          sub           sub : 81920
          sub        select : 81920
         icmp           sub : 81920
         icmp            br : 45140
          mul           add : 40970
getelementptr          call : 40961
       select        select : 40960
          mul           mul : 40960
          mul          icmp : 40960
         load           mul : 40960
         load getelementptr : 40960
         load          call : 40960
         icmp          icmp : 40960
         icmp           and : 40960
          and        select : 40960
        trunc            br : 4160
         icmp         trunc : 4096
         load            br : 612
getelementptr       bitcast : 601
      bitcast          load : 601
          shl           xor : 100
getelementptr         store : 80
          xor           shl : 50
          xor          lshr : 50
        store           shl : 50
         lshr           xor : 50
          xor         trunc : 30
        trunc getelementptr : 30
          xor           and : 20
        store getelementptr : 20
          and getelementptr : 20
        store            br : 10
        store           add : 10
         sext           mul : 10
         load          sext : 10
         load          icmp : 10
         icmp getelementptr : 10
getelementptr          icmp : 10
          add         store : 10
        store           ret : 1
      bitcast          call : 1
       alloca       bitcast : 1
```

Для трех:
```
          sub           sub        select : 81920
          sub        select          call : 81920
         icmp           sub           sub : 81920
       select        select          call : 40960
          mul           mul           add : 40960
          mul          icmp          icmp : 40960
          mul           add          call : 40960
         load           mul          icmp : 40960
         load getelementptr          load : 40960
         icmp          icmp           and : 40960
         icmp           and        select : 40960
getelementptr          load           mul : 40960
getelementptr          load getelementptr : 40960
getelementptr          load          call : 40960
          and        select        select : 40960
         icmp         trunc            br : 4096
getelementptr       bitcast          load : 601
      bitcast          load            br : 601
          xor           shl           xor : 50
          xor          lshr           xor : 50
        store           shl           xor : 50
          shl           xor          lshr : 50
         lshr           xor           shl : 50
getelementptr         store           shl : 50
          xor         trunc getelementptr : 30
        trunc getelementptr         store : 30
          shl           xor         trunc : 30
          xor           and getelementptr : 20
        store getelementptr         store : 20
          shl           xor           and : 20
getelementptr         store getelementptr : 20
          and getelementptr         store : 20
        store           add          call : 10
         sext           mul           add : 10
          mul           add         store : 10
         load          sext           mul : 10
         load          icmp getelementptr : 10
         icmp getelementptr          load : 10
getelementptr         store           add : 10
getelementptr          load          sext : 10
getelementptr          load          icmp : 10
getelementptr          load            br : 10
getelementptr          icmp            br : 10
          add         store            br : 10
       alloca       bitcast          call : 1
```

```
         icmp           sub           sub        select          call : 81920
          mul          icmp          icmp           and        select : 40960
         load           mul          icmp          icmp           and : 40960
         icmp          icmp           and        select        select : 40960
         icmp           and        select        select          call : 40960
getelementptr          load           mul          icmp          icmp : 40960
getelementptr          load getelementptr          load          call : 40960
          xor          lshr           xor           shl           xor : 50
        store           shl           xor          lshr           xor : 50
          shl           xor          lshr           xor           shl : 50
getelementptr         store           shl           xor          lshr : 50
          xor           shl           xor         trunc getelementptr : 30
          shl           xor         trunc getelementptr         store : 30
         lshr           xor           shl           xor         trunc : 30
          xor         trunc getelementptr         store           shl : 20
          xor           shl           xor           and getelementptr : 20
          xor           and getelementptr         store           shl : 20
        trunc getelementptr         store           shl           xor : 20
          shl           xor           and getelementptr         store : 20
         lshr           xor           shl           xor           and : 20
          and getelementptr         store           shl           xor : 20
          xor         trunc getelementptr         store           add : 10
        trunc getelementptr         store           add          call : 10
        store getelementptr         store           shl           xor : 10
        store getelementptr         store getelementptr         store : 10
         sext           mul           add         store            br : 10
         load          sext           mul           add         store : 10
         load          icmp getelementptr          load            br : 10
getelementptr         store getelementptr         store           shl : 10
getelementptr         store getelementptr         store getelementptr : 10
getelementptr          load          sext           mul           add : 10
getelementptr          load          icmp getelementptr          load : 10
```

Много call инструкций - следствие вызова вспомогательных функций llvm(например lifetime.start)

## Анализ трасс с меньшим количеством кода
Из-за константного количества точек многие циклы были заинлайнены.
Поэтому чтобы не генерировать руками заиндайненый код, были добавлены:
- опция компиляция `-fno-unroll-loops`
- атрибуты `__attribute__((noinline))`

Для одиночных инструкций:
```
getelementptr : 164582
         icmp : 131176
         load : 123511
       select : 81920
           br : 54030
         call : 49228
          add : 45150
          and : 40980
          mul : 40970
        trunc : 4190
          ret : 4119
      bitcast : 602
        store : 90
         sext : 10
       alloca : 1
```

Для 2 инструкций:
```
getelementptr          load : 122910
         icmp            br : 45150
          add          icmp : 45140
       select        select : 40960
       select           add : 40960
          mul          icmp : 40960
         load           mul : 40960
         load getelementptr : 40960
         load          call : 40960
         icmp          icmp : 40960
         icmp           and : 40960
getelementptr getelementptr : 40960
          and        select : 40960
        trunc            br : 4160
         icmp         trunc : 4096
         load            br : 611
getelementptr       bitcast : 601
      bitcast          load : 601
getelementptr         store : 80
        store          call : 50
        trunc getelementptr : 30
getelementptr          call : 21
        store getelementptr : 20
          and getelementptr : 20
        store           ret : 10
        store            br : 10
         sext           mul : 10
          mul           add : 10
         load          sext : 10
         load          icmp : 10
         icmp getelementptr : 10
getelementptr          icmp : 10
          add         store : 10
      bitcast          call : 1
       alloca       bitcast : 1
```

Для 3 интсрукций:
```
          add          icmp            br : 45140
       select        select           add : 40960
       select           add          icmp : 40960
          mul          icmp          icmp : 40960
         load           mul          icmp : 40960
         load getelementptr          load : 40960
         icmp          icmp           and : 40960
         icmp           and        select : 40960
getelementptr          load           mul : 40960
getelementptr          load getelementptr : 40960
getelementptr          load          call : 40960
getelementptr getelementptr          load : 40960
          and        select        select : 40960
         icmp         trunc            br : 4096
getelementptr       bitcast          load : 601
      bitcast          load            br : 601
getelementptr         store          call : 50
        trunc getelementptr         store : 30
        store getelementptr         store : 20
getelementptr         store getelementptr : 20
          and getelementptr         store : 20
         sext           mul           add : 10
          mul           add         store : 10
         load          sext           mul : 10
         load          icmp getelementptr : 10
         icmp getelementptr          load : 10
getelementptr         store           ret : 10
getelementptr          load          sext : 10
getelementptr          load          icmp : 10
getelementptr          load            br : 10
getelementptr          icmp            br : 10
          add         store            br : 10
       alloca       bitcast          call : 1
```

Для 5:
```
       select        select           add          icmp            br : 40960
          mul          icmp          icmp           and        select : 40960
         load           mul          icmp          icmp           and : 40960
         icmp          icmp           and        select        select : 40960
         icmp           and        select        select           add : 40960
getelementptr          load           mul          icmp          icmp : 40960
getelementptr          load getelementptr          load          call : 40960
getelementptr getelementptr          load getelementptr          load : 40960
          and        select        select           add          icmp : 40960
        store getelementptr         store getelementptr         store : 10
         sext           mul           add         store            br : 10
         load          sext           mul           add         store : 10
         load          icmp getelementptr          load            br : 10
getelementptr         store getelementptr         store getelementptr : 10
getelementptr         store getelementptr         store          call : 10
getelementptr          load          sext           mul           add : 10
getelementptr          load          icmp getelementptr          load : 10
```