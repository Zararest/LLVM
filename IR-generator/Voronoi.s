.global
  Seed 1

.text
#a0 - dot struct# 
<changePosition>
  storeDotFiledImm a0, 4, 0
  storeDotFiledImm a0, 2, 5
  storeDotFiledImm a0, 3, 1
  xorshift Seed -> t0
  and t0, 63 -> t0
  storeDotFiled a0, 0, t0
  xorshift Seed, -> t1
  and t1, 63 -> t1  
  storeDotFiled a0, 1, t1
  initRgb a0, Seed
  ret


#a0 - dot struct# 
<updateDot>
  loadDotFiled a0, 3 -> t0
  cmpEqImm t0, -1 -> t1
  loadDotFiled a0, 4 -> t2
  brCond t1, BB1, BB3
BB1:
  jumpIfDot a0, 2, t2, BB3, BB2
BB2: #pred: entry# 
  #регситр a0 уже правильный#
  call changePosition
  br BB6
BB3: #pred: entry, BB1#
  #геп перенесен к стору#
  cmpUGT t2, 64 -> t3
  brCond t3, BB4, BB5
BB4: #pred: BB3#
  storeDotFiledImm a0, 3, -1 -> t4
  #Phi node in BB5#
  li -1 -> t0
  br BB5
BB5:
  #Phi is t0#
  loadDotFiled a0, 2 -> t5
  #removed sext#
  mul t5, t0 -> t6
  add t6, t2 -> t7
  storeDotField a0, 4, t7
  br BB6
BB6:
  ret


<changeState>
  #Phi in BB2#
  li 0, t0
  br BB2 
BB1:
  ret
BB2:
  getDotAddr a0, t0 -> a0
  call updateDot
  incJump t0, 10, BB1, BB2 -> t0


<distance>
  cmpUGT a2, a0 -> t0
  sub a2, a0 -> t1
  sub a0, a2 -> t2
  select t0, t1, t2 -> t3
  cmpUGT a3, a1 -> t4
  sub a3, a1 -> t5
  sub a1, a3 -> t6
  select t4, t5, t6 -> t7
  norm t3, t7 -> r0
  ret


<getNearestDot>
  #nullptr init#
  li 0 -> t0
  li 48830 -> t1
  li 0 -> t2
  br BB2
BB1:
  ret 

BB2:
  getDotAddr a2, t2 -> t3
  loadDotFiled t3, 0 -> t4
  loadDotFiled t3, 1 -> a3
  #спил регистров# 
  mv a0 -> t10
  mv a1 -> t11
  mv a2 -> t6
  mv t4 -> a2
  call distance
  mv t10 -> a0
  mv t11 -> a1
  mv t6 -> a2
  loadDotFiled t3, 4 -> t7
  mul t7, t7 -> t8
  cmpTwo r0, t8, t1 -> t9
  select t9, r0, t1 -> t1
  select t9, t3, t0 -> t0
  incJump t2, 10, BB1, BB2 -> t2


<drawFrame>
  #t0 - X t2 - Y t4 - RGB#
  li 0 -> t0
  br BB1
BB1:  
  li 0 -> t2
  br BB4
BB2:
  ret
BB3:
  incJump t0, 64, BB2, BB1 -> t0
BB4:
  #спил регистров#
  mv a0 -> t1
  mv t0 -> a0
  mv t2 -> a1
  mv t1 -> a2
  call getNearestDot
  mv t1 -> a0
  cmpEQImm r0, 0 -> t3
  li t4, 657930
  brCond t3, BB6, BB5
BB5:
  loadRgb r0 -> t4
  br BB6
BB6:
  mv a0 -> t5
  mv t0 -> a0
  mv t2 -> a1
  mv t4 -> a2
  call simPutPixel
  mv t5 -> a0
  incJump t2, 64, BB3, BB4 -> t2


<initDots>
  li 0 -> t0
  br BB2
BB1:
  ret
BB2:
  getDotAddr a0, t0 -> t1
  mv a0 -> t2
  mv t1 -> a0
  call changePosition
  mv t2 -> a0
  incJmp t0, 10, BB1, BB2 -> t0

<app>
  createDots 10 -> t0
  mv t0 -> a0
  call initDots
  li 0 -> t1
  br BB2
BB1:
  ret
BB2:
  mv t0 -> a0
  call changeState
  mv t0 -> a0
  call drawFrame
  call simFlush
  incJmp t1, 100, BB1, BB2 -> t1