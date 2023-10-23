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