# General concepts
There are 3 main stages in compiler:
- frontend: initial lang parsing
- middleend: machine independent representation (e.g. LLVM IR)
- backend: machine dependent reprersentation
- assembler: assembler

## Backend
The main feature of this representation is `absence of SSA` form.
The pass that destroys it is the register allocation.

### middleend -> backend
middleend:
```
define i32 @foo(i32 %x, i32 %y) #0 {
  entry:
  %add = add nsw i32 %x, %y
  ret i32 1
}
```
|
V
`Instruction selection` - LLVM IR has machine independent instructions, this pass selects instructions for the specific arch in order to create sequence of machine instructions with the same semantics (on this stage there is still SSA form, since all instructions has virtual registers)
|
V
```
Function Live Ins: $edi in %0, $esi in %2 
(here we have real registers because of calling convention)
bb.0.entry:
  liveins: $edi, $esi
  %2:gr32 = COPY $esi
  %0:gr32 = COPY $edi
  %1:gr32 = COPY killed %0:gr32
  %3:gr32 = COPY killed %2:gr32
  %4:gr32 = MOV32ri 1
  $eax = COPY %4:gr32
  RET64 implicit $eax
```
|
V
`Register allocation` - selects registers in a way there are minimal number of spills(store register to the memory and that load this value later)
|
V
```
Function Live Ins: $edi, $esi

bb.0.entry:
  liveins: $edi, $esi
  renamable $eax = MOV32ri 1
  RET64 implicit killed $eax
```

### Backend optimizations
The main feature of such optimizations is that they are machine aware.

### Scheduling
One of the most important optimizations is `Scheduling`.
This pass reorders instructions so that particular CPU will execute them faster.
For instance, compiler could calculate approximate duration for each instructions and promote slowest to the beginning of the function in order to let other instructions(which don't have data dependencies) exucute out of order.

### Copy propagation
In some cases there are instructions that do nothing:
```
bb1:
  a1 = a2
  ..no def of a1 or a2..
  a2 = a1
```
The second instruction is useless and so could be removed.
`MachineCopyPropagation` pass does this.

Such istructions is an artifact of instruction selection and register allocation.

## Other examples
- insert cache/branch predictors' intrinsics
- creation of immidiates in a smart way
- vectorization optimization

Use ` -debug-pass=Structure` and check passes after RegAlloc

## backend -> assembler
This stage also has some optimizations, but they are minor.

`AsmPrinter` is a class that translates machine IR to the assembler.
It could optimize some instructions.
For example in some cases RISC-V ordinary instructions might be translated as compressed.