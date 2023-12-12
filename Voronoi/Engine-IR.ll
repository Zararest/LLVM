!<arch>
Engine.c.o/     0           0     0     644     37700     `
; ModuleID = '/home/uwu/Proga/LLVM-course/Voronoi/Engine.c'
source_filename = "/home/uwu/Proga/LLVM-course/Voronoi/Engine.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.RGB = type { i8, i8, i8 }
%struct.Dot = type { i64, i64, i64, i64, i64, %struct.RGB }

@BackGround = dso_local local_unnamed_addr constant %struct.RGB { i8 10, i8 10, i8 10 }, align 1, !dbg !0
@xorshift.Seed = internal unnamed_addr global i64 1, align 8, !dbg !6

; Function Attrs: nofree noinline norecurse nounwind uwtable willreturn
define dso_local i64 @xorshift() local_unnamed_addr #0 !dbg !8 {
  %1 = load i64, i64* @xorshift.Seed, align 8, !dbg !31, !tbaa !32
  %2 = shl i64 %1, 13, !dbg !36
  %3 = xor i64 %2, %1, !dbg !37
  %4 = lshr i64 %3, 7, !dbg !38
  %5 = xor i64 %4, %3, !dbg !39
  %6 = shl i64 %5, 17, !dbg !40
  %7 = xor i64 %6, %5, !dbg !41
  store i64 %7, i64* @xorshift.Seed, align 8, !dbg !41, !tbaa !32
  ret i64 %7, !dbg !42
}

; Function Attrs: norecurse nounwind readnone uwtable willreturn
define dso_local void @dumpDots(%struct.Dot* nocapture %0) local_unnamed_addr #1 !dbg !43 {
  call void @llvm.dbg.value(metadata %struct.Dot* %0, metadata !58, metadata !DIExpression()), !dbg !59
  ret void, !dbg !60
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #2

; Function Attrs: nofree noinline norecurse nounwind uwtable willreturn
define dso_local void @changePosition(%struct.Dot* nocapture %0) local_unnamed_addr #0 !dbg !61 {
  call void @llvm.dbg.value(metadata %struct.Dot* %0, metadata !66, metadata !DIExpression()), !dbg !67
  %2 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 4, !dbg !68
  store i64 0, i64* %2, align 8, !dbg !69, !tbaa !70
  %3 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 2, !dbg !74
  %4 = bitcast i64* %3 to <2 x i64>*, !dbg !75
  store <2 x i64> <i64 5, i64 1>, <2 x i64>* %4, align 8, !dbg !75, !tbaa !76
  %5 = tail call i64 @xorshift(), !dbg !77
  %6 = and i64 %5, 63, !dbg !78
  %7 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 0, !dbg !79
  store i64 %6, i64* %7, align 8, !dbg !80, !tbaa !81
  %8 = tail call i64 @xorshift(), !dbg !82
  %9 = and i64 %8, 63, !dbg !83
  %10 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 1, !dbg !84
  store i64 %9, i64* %10, align 8, !dbg !85, !tbaa !86
  %11 = tail call i64 @xorshift(), !dbg !87
  %12 = trunc i64 %11 to i8, !dbg !87
  %13 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 5, i32 0, !dbg !88
  store i8 %12, i8* %13, align 8, !dbg !89, !tbaa !90
  %14 = tail call i64 @xorshift(), !dbg !91
  %15 = trunc i64 %14 to i8, !dbg !91
  %16 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 5, i32 1, !dbg !92
  store i8 %15, i8* %16, align 1, !dbg !93, !tbaa !94
  %17 = tail call i64 @xorshift(), !dbg !95
  %18 = trunc i64 %17 to i8, !dbg !95
  %19 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 5, i32 2, !dbg !96
  store i8 %18, i8* %19, align 2, !dbg !97, !tbaa !98
  ret void, !dbg !99
}

; Function Attrs: nofree noinline norecurse nounwind uwtable willreturn
define dso_local void @updateDot(%struct.Dot* nocapture %0) local_unnamed_addr #0 !dbg !100 {
  call void @llvm.dbg.value(metadata %struct.Dot* %0, metadata !102, metadata !DIExpression()), !dbg !103
  %2 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 3, !dbg !104
  %3 = load i64, i64* %2, align 8, !dbg !104, !tbaa !106
  %4 = icmp eq i64 %3, -1, !dbg !107
  %5 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 4
  %6 = load i64, i64* %5, align 8, !dbg !103, !tbaa !70
  br i1 %4, label %7, label %12, !dbg !108

7:                                                ; preds = %1
  %8 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 2, !dbg !109
  %9 = load i64, i64* %8, align 8, !dbg !109, !tbaa !110
  %10 = icmp ugt i64 %6, %9, !dbg !111
  br i1 %10, label %12, label %11, !dbg !112

11:                                               ; preds = %7
  tail call void @changePosition(%struct.Dot* nonnull %0), !dbg !113
  br label %22, !dbg !115

12:                                               ; preds = %1, %7
  %13 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 4, !dbg !116
  %14 = icmp ugt i64 %6, 64, !dbg !118
  br i1 %14, label %15, label %16, !dbg !119

15:                                               ; preds = %12
  store i64 -1, i64* %2, align 8, !dbg !120, !tbaa !106
  br label %16, !dbg !121

16:                                               ; preds = %15, %12
  %17 = phi i64 [ -1, %15 ], [ %3, %12 ], !dbg !122
  %18 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 2, !dbg !123
  %19 = load i64, i64* %18, align 8, !dbg !123, !tbaa !110
  %20 = mul i64 %17, %19, !dbg !124
  %21 = add i64 %20, %6, !dbg !125
  store i64 %21, i64* %13, align 8, !dbg !125, !tbaa !70
  br label %22, !dbg !126

22:                                               ; preds = %16, %11
  ret void, !dbg !126
}

; Function Attrs: nofree noinline norecurse nounwind uwtable
define dso_local void @changeState(%struct.Dot* nocapture %0) local_unnamed_addr #3 !dbg !127 {
  call void @llvm.dbg.value(metadata %struct.Dot* %0, metadata !129, metadata !DIExpression()), !dbg !133
  call void @llvm.dbg.value(metadata i32 0, metadata !130, metadata !DIExpression()), !dbg !134
  br label %3, !dbg !135

2:                                                ; preds = %3
  ret void, !dbg !136

3:                                                ; preds = %1, %3
  %4 = phi i64 [ 0, %1 ], [ %6, %3 ]
  call void @llvm.dbg.value(metadata i64 %4, metadata !130, metadata !DIExpression()), !dbg !134
  %5 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 %4, !dbg !137
  tail call void @updateDot(%struct.Dot* %5), !dbg !139
  %6 = add nuw nsw i64 %4, 1, !dbg !140
  call void @llvm.dbg.value(metadata i64 %6, metadata !130, metadata !DIExpression()), !dbg !134
  %7 = icmp eq i64 %6, 10, !dbg !141
  br i1 %7, label %2, label %3, !dbg !135, !llvm.loop !142
}

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #4

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #4

; Function Attrs: noinline norecurse nounwind readnone uwtable willreturn
define dso_local i64 @distance(i64 %0, i64 %1, i64 %2, i64 %3) local_unnamed_addr #5 !dbg !146 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !150, metadata !DIExpression()), !dbg !156
  call void @llvm.dbg.value(metadata i64 %1, metadata !151, metadata !DIExpression()), !dbg !156
  call void @llvm.dbg.value(metadata i64 %2, metadata !152, metadata !DIExpression()), !dbg !156
  call void @llvm.dbg.value(metadata i64 %3, metadata !153, metadata !DIExpression()), !dbg !156
  %5 = icmp ugt i64 %2, %0, !dbg !157
  %6 = sub i64 %2, %0, !dbg !158
  %7 = sub i64 %0, %2, !dbg !158
  %8 = select i1 %5, i64 %6, i64 %7, !dbg !158
  call void @llvm.dbg.value(metadata i64 %8, metadata !154, metadata !DIExpression()), !dbg !156
  %9 = icmp ugt i64 %3, %1, !dbg !159
  %10 = sub i64 %3, %1, !dbg !160
  %11 = sub i64 %1, %3, !dbg !160
  %12 = select i1 %9, i64 %10, i64 %11, !dbg !160
  call void @llvm.dbg.value(metadata i64 %12, metadata !155, metadata !DIExpression()), !dbg !156
  %13 = mul i64 %8, %8, !dbg !161
  %14 = mul i64 %12, %12, !dbg !162
  %15 = add i64 %14, %13, !dbg !163
  ret i64 %15, !dbg !164
}

; Function Attrs: noinline norecurse nounwind readonly uwtable
define dso_local %struct.Dot* @getNearestDot(i64 %0, i64 %1, %struct.Dot* readonly %2) local_unnamed_addr #6 !dbg !165 {
  call void @llvm.dbg.value(metadata i64 %0, metadata !169, metadata !DIExpression()), !dbg !180
  call void @llvm.dbg.value(metadata i64 %1, metadata !170, metadata !DIExpression()), !dbg !180
  call void @llvm.dbg.value(metadata %struct.Dot* %2, metadata !171, metadata !DIExpression()), !dbg !180
  call void @llvm.dbg.value(metadata %struct.Dot* null, metadata !172, metadata !DIExpression()), !dbg !180
  call void @llvm.dbg.value(metadata i64 48830, metadata !173, metadata !DIExpression()), !dbg !180
  call void @llvm.dbg.value(metadata i64 0, metadata !174, metadata !DIExpression()), !dbg !181
  br label %5, !dbg !182

4:                                                ; preds = %5
  ret %struct.Dot* %22, !dbg !183

5:                                                ; preds = %3, %5
  %6 = phi %struct.Dot* [ null, %3 ], [ %22, %5 ]
  %7 = phi i64 [ 48830, %3 ], [ %21, %5 ]
  %8 = phi i64 [ 0, %3 ], [ %23, %5 ]
  call void @llvm.dbg.value(metadata %struct.Dot* %6, metadata !172, metadata !DIExpression()), !dbg !180
  call void @llvm.dbg.value(metadata i64 %7, metadata !173, metadata !DIExpression()), !dbg !180
  call void @llvm.dbg.value(metadata i64 %8, metadata !174, metadata !DIExpression()), !dbg !181
  %9 = getelementptr inbounds %struct.Dot, %struct.Dot* %2, i64 %8, !dbg !184
  call void @llvm.dbg.value(metadata %struct.Dot* %9, metadata !176, metadata !DIExpression()), !dbg !185
  %10 = getelementptr inbounds %struct.Dot, %struct.Dot* %9, i64 0, i32 0, !dbg !186
  %11 = load i64, i64* %10, align 8, !dbg !186, !tbaa !81
  %12 = getelementptr inbounds %struct.Dot, %struct.Dot* %2, i64 %8, i32 1, !dbg !187
  %13 = load i64, i64* %12, align 8, !dbg !187, !tbaa !86
  %14 = tail call i64 @distance(i64 %0, i64 %1, i64 %11, i64 %13), !dbg !188
  call void @llvm.dbg.value(metadata i64 %14, metadata !179, metadata !DIExpression()), !dbg !185
  %15 = getelementptr inbounds %struct.Dot, %struct.Dot* %2, i64 %8, i32 4, !dbg !189
  %16 = load i64, i64* %15, align 8, !dbg !189, !tbaa !70
  %17 = mul i64 %16, %16, !dbg !191
  %18 = icmp ult i64 %14, %17, !dbg !192
  %19 = icmp ult i64 %14, %7
  %20 = and i1 %19, %18, !dbg !193
  %21 = select i1 %20, i64 %14, i64 %7, !dbg !193
  %22 = select i1 %20, %struct.Dot* %9, %struct.Dot* %6, !dbg !193
  call void @llvm.dbg.value(metadata %struct.Dot* %22, metadata !172, metadata !DIExpression()), !dbg !180
  call void @llvm.dbg.value(metadata i64 %21, metadata !173, metadata !DIExpression()), !dbg !180
  %23 = add nuw nsw i64 %8, 1, !dbg !194
  call void @llvm.dbg.value(metadata i64 %23, metadata !174, metadata !DIExpression()), !dbg !181
  %24 = icmp eq i64 %23, 10, !dbg !195
  br i1 %24, label %4, label %5, !dbg !182, !llvm.loop !196
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @drawFrame(%struct.Dot* readonly %0) local_unnamed_addr #7 !dbg !198 {
  call void @llvm.dbg.value(metadata %struct.Dot* %0, metadata !200, metadata !DIExpression()), !dbg !209
  call void @llvm.dbg.value(metadata i64 0, metadata !201, metadata !DIExpression()), !dbg !210
  br label %2, !dbg !211

2:                                                ; preds = %1, %6
  %3 = phi i64 [ 0, %1 ], [ %7, %6 ]
  call void @llvm.dbg.value(metadata i64 %3, metadata !201, metadata !DIExpression()), !dbg !210
  call void @llvm.dbg.value(metadata i64 0, metadata !203, metadata !DIExpression()), !dbg !212
  %4 = trunc i64 %3 to i32
  br label %9, !dbg !213

5:                                                ; preds = %6
  ret void, !dbg !214

6:                                                ; preds = %18
  %7 = add nuw nsw i64 %3, 1, !dbg !215
  call void @llvm.dbg.value(metadata i64 %7, metadata !201, metadata !DIExpression()), !dbg !210
  %8 = icmp eq i64 %7, 64, !dbg !216
  br i1 %8, label %5, label %2, !dbg !211, !llvm.loop !217

9:                                                ; preds = %2, %18
  %10 = phi i64 [ 0, %2 ], [ %20, %18 ]
  call void @llvm.dbg.value(metadata i64 %10, metadata !203, metadata !DIExpression()), !dbg !212
  %11 = tail call %struct.Dot* @getNearestDot(i64 %3, i64 %10, %struct.Dot* %0), !dbg !219
  call void @llvm.dbg.value(metadata %struct.Dot* %11, metadata !206, metadata !DIExpression()), !dbg !220
  %12 = icmp eq %struct.Dot* %11, null, !dbg !221
  %13 = trunc i64 %10 to i32, !dbg !220
  br i1 %12, label %18, label %14, !dbg !223

14:                                               ; preds = %9
  %15 = getelementptr inbounds %struct.Dot, %struct.Dot* %11, i64 0, i32 5, !dbg !224
  %16 = bitcast %struct.RGB* %15 to i24*, !dbg !224
  %17 = load i24, i24* %16, align 8, !dbg !224
  br label %18, !dbg !225

18:                                               ; preds = %9, %14
  %19 = phi i24 [ %17, %14 ], [ 657930, %9 ]
  tail call void @simPutPixel(i32 %4, i32 %13, i24 %19) #10, !dbg !220
  %20 = add nuw nsw i64 %10, 1, !dbg !226
  %21 = icmp eq i64 %20, 64, !dbg !227
  br i1 %21, label %6, label %9, !dbg !213, !llvm.loop !228
}

declare !dbg !230 dso_local void @simPutPixel(i32, i32, i24) local_unnamed_addr #8

; Function Attrs: nofree noinline norecurse nounwind uwtable
define dso_local void @initDots(%struct.Dot* nocapture %0) local_unnamed_addr #3 !dbg !233 {
  call void @llvm.dbg.value(metadata %struct.Dot* %0, metadata !235, metadata !DIExpression()), !dbg !238
  call void @llvm.dbg.value(metadata i32 0, metadata !236, metadata !DIExpression()), !dbg !239
  br label %3, !dbg !240

2:                                                ; preds = %3
  ret void, !dbg !241

3:                                                ; preds = %1, %3
  %4 = phi i64 [ 0, %1 ], [ %6, %3 ]
  call void @llvm.dbg.value(metadata i64 %4, metadata !236, metadata !DIExpression()), !dbg !239
  %5 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 %4, !dbg !242
  tail call void @changePosition(%struct.Dot* %5), !dbg !244
  %6 = add nuw nsw i64 %4, 1, !dbg !245
  call void @llvm.dbg.value(metadata i64 %6, metadata !236, metadata !DIExpression()), !dbg !239
  %7 = icmp eq i64 %6, 10, !dbg !246
  br i1 %7, label %2, label %3, !dbg !240, !llvm.loop !247
}

; Function Attrs: nounwind uwtable
define dso_local void @app() local_unnamed_addr #9 !dbg !249 {
  %1 = alloca [10 x %struct.Dot], align 16
  %2 = bitcast [10 x %struct.Dot]* %1 to i8*, !dbg !259
  call void @llvm.lifetime.start.p0i8(i64 480, i8* nonnull %2) #10, !dbg !259
  call void @llvm.dbg.declare(metadata [10 x %struct.Dot]* %1, metadata !253, metadata !DIExpression()), !dbg !260
  %3 = getelementptr inbounds [10 x %struct.Dot], [10 x %struct.Dot]* %1, i64 0, i64 0, !dbg !261
  call void @initDots(%struct.Dot* nonnull %3), !dbg !262
  call void @changeState(%struct.Dot* nonnull %3), !dbg !264
  call void @drawFrame(%struct.Dot* nonnull %3), !dbg !267
  call void (...) @simFlush() #10, !dbg !268
  ret void, !dbg !269
}

declare !dbg !270 dso_local void @simFlush(...) local_unnamed_addr #8

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.value(metadata, metadata, metadata) #2

attributes #0 = { nofree noinline norecurse nounwind uwtable willreturn "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { norecurse nounwind readnone uwtable willreturn "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #3 = { nofree noinline norecurse nounwind uwtable "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { argmemonly nofree nosync nounwind willreturn }
attributes #5 = { noinline norecurse nounwind readnone uwtable willreturn "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { noinline norecurse nounwind readonly uwtable "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { noinline nounwind uwtable "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #8 = { "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #9 = { nounwind uwtable "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #10 = { nounwind }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!27, !28, !29}
!llvm.ident = !{!30}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "BackGround", scope: !2, file: !9, line: 14, type: !17, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C99, file: !3, producer: "Ubuntu clang version 12.0.0-3ubuntu1~20.04.5", isOptimized: true, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5, splitDebugInlining: false, nameTableKind: None)
!3 = !DIFile(filename: "/home/uwu/Proga/LLVM-course/Voronoi/Engine.c", directory: "/home/uwu/Proga/LLVM-course/Voronoi/build")
!4 = !{}
!5 = !{!0, !6}
!6 = !DIGlobalVariableExpression(var: !7, expr: !DIExpression())
!7 = distinct !DIGlobalVariable(name: "Seed", scope: !8, file: !9, line: 33, type: !12, isLocal: true, isDefinition: true)
!8 = distinct !DISubprogram(name: "xorshift", scope: !9, file: !9, line: 32, type: !10, scopeLine: 32, flags: DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !4)
!9 = !DIFile(filename: "Engine.c", directory: "/home/uwu/Proga/LLVM-course/Voronoi")
!10 = !DISubroutineType(types: !11)
!11 = !{!12}
!12 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint64_t", file: !13, line: 27, baseType: !14)
!13 = !DIFile(filename: "/usr/include/x86_64-linux-gnu/bits/stdint-uintn.h", directory: "")
!14 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint64_t", file: !15, line: 45, baseType: !16)
!15 = !DIFile(filename: "/usr/include/x86_64-linux-gnu/bits/types.h", directory: "")
!16 = !DIBasicType(name: "long unsigned int", size: 64, encoding: DW_ATE_unsigned)
!17 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !18)
!18 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "RGB", file: !19, line: 6, size: 24, elements: !20)
!19 = !DIFile(filename: "lib/Graphics.h", directory: "/home/uwu/Proga/LLVM-course/Voronoi")
!20 = !{!21, !25, !26}
!21 = !DIDerivedType(tag: DW_TAG_member, name: "R", scope: !18, file: !19, line: 7, baseType: !22, size: 8)
!22 = !DIDerivedType(tag: DW_TAG_typedef, name: "uint8_t", file: !13, line: 24, baseType: !23)
!23 = !DIDerivedType(tag: DW_TAG_typedef, name: "__uint8_t", file: !15, line: 38, baseType: !24)
!24 = !DIBasicType(name: "unsigned char", size: 8, encoding: DW_ATE_unsigned_char)
!25 = !DIDerivedType(tag: DW_TAG_member, name: "G", scope: !18, file: !19, line: 8, baseType: !22, size: 8, offset: 8)
!26 = !DIDerivedType(tag: DW_TAG_member, name: "B", scope: !18, file: !19, line: 9, baseType: !22, size: 8, offset: 16)
!27 = !{i32 7, !"Dwarf Version", i32 4}
!28 = !{i32 2, !"Debug Info Version", i32 3}
!29 = !{i32 1, !"wchar_size", i32 4}
!30 = !{!"Ubuntu clang version 12.0.0-3ubuntu1~20.04.5"}
!31 = !DILocation(line: 34, column: 11, scope: !8)
!32 = !{!33, !33, i64 0}
!33 = !{!"long", !34, i64 0}
!34 = !{!"omnipotent char", !35, i64 0}
!35 = !{!"Simple C/C++ TBAA"}
!36 = !DILocation(line: 34, column: 16, scope: !8)
!37 = !DILocation(line: 34, column: 8, scope: !8)
!38 = !DILocation(line: 35, column: 16, scope: !8)
!39 = !DILocation(line: 35, column: 8, scope: !8)
!40 = !DILocation(line: 36, column: 16, scope: !8)
!41 = !DILocation(line: 36, column: 8, scope: !8)
!42 = !DILocation(line: 37, column: 3, scope: !8)
!43 = distinct !DISubprogram(name: "dumpDots", scope: !9, file: !9, line: 40, type: !44, scopeLine: 40, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !57)
!44 = !DISubroutineType(types: !45)
!45 = !{null, !46}
!46 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !47, size: 64)
!47 = !DIDerivedType(tag: DW_TAG_const_type, baseType: !48)
!48 = distinct !DICompositeType(tag: DW_TAG_structure_type, name: "Dot", file: !9, line: 17, size: 384, elements: !49)
!49 = !{!50, !51, !52, !53, !55, !56}
!50 = !DIDerivedType(tag: DW_TAG_member, name: "X", scope: !48, file: !9, line: 19, baseType: !12, size: 64)
!51 = !DIDerivedType(tag: DW_TAG_member, name: "Y", scope: !48, file: !9, line: 20, baseType: !12, size: 64, offset: 64)
!52 = !DIDerivedType(tag: DW_TAG_member, name: "GrowthSpeed", scope: !48, file: !9, line: 22, baseType: !12, size: 64, offset: 128)
!53 = !DIDerivedType(tag: DW_TAG_member, name: "GrowthDirection", scope: !48, file: !9, line: 24, baseType: !54, size: 64, offset: 192)
!54 = !DIBasicType(name: "long long int", size: 64, encoding: DW_ATE_signed)
!55 = !DIDerivedType(tag: DW_TAG_member, name: "R", scope: !48, file: !9, line: 26, baseType: !12, size: 64, offset: 256)
!56 = !DIDerivedType(tag: DW_TAG_member, name: "Colour", scope: !48, file: !9, line: 28, baseType: !18, size: 24, offset: 320)
!57 = !{!58}
!58 = !DILocalVariable(name: "ArrayOfDots", arg: 1, scope: !43, file: !9, line: 40, type: !46)
!59 = !DILocation(line: 0, scope: !43)
!60 = !DILocation(line: 54, column: 1, scope: !43)
!61 = distinct !DISubprogram(name: "changePosition", scope: !9, file: !9, line: 57, type: !62, scopeLine: 57, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !65)
!62 = !DISubroutineType(types: !63)
!63 = !{null, !64}
!64 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !48, size: 64)
!65 = !{!66}
!66 = !DILocalVariable(name: "CurDot", arg: 1, scope: !61, file: !9, line: 57, type: !64)
!67 = !DILocation(line: 0, scope: !61)
!68 = !DILocation(line: 58, column: 11, scope: !61)
!69 = !DILocation(line: 58, column: 13, scope: !61)
!70 = !{!71, !33, i64 32}
!71 = !{!"Dot", !33, i64 0, !33, i64 8, !33, i64 16, !72, i64 24, !33, i64 32, !73, i64 40}
!72 = !{!"long long", !34, i64 0}
!73 = !{!"RGB", !34, i64 0, !34, i64 1, !34, i64 2}
!74 = !DILocation(line: 59, column: 11, scope: !61)
!75 = !DILocation(line: 59, column: 23, scope: !61)
!76 = !{!34, !34, i64 0}
!77 = !DILocation(line: 61, column: 15, scope: !61)
!78 = !DILocation(line: 61, column: 26, scope: !61)
!79 = !DILocation(line: 61, column: 11, scope: !61)
!80 = !DILocation(line: 61, column: 13, scope: !61)
!81 = !{!71, !33, i64 0}
!82 = !DILocation(line: 62, column: 15, scope: !61)
!83 = !DILocation(line: 62, column: 26, scope: !61)
!84 = !DILocation(line: 62, column: 11, scope: !61)
!85 = !DILocation(line: 62, column: 13, scope: !61)
!86 = !{!71, !33, i64 8}
!87 = !DILocation(line: 63, column: 22, scope: !61)
!88 = !DILocation(line: 63, column: 18, scope: !61)
!89 = !DILocation(line: 63, column: 20, scope: !61)
!90 = !{!71, !34, i64 40}
!91 = !DILocation(line: 64, column: 22, scope: !61)
!92 = !DILocation(line: 64, column: 18, scope: !61)
!93 = !DILocation(line: 64, column: 20, scope: !61)
!94 = !{!71, !34, i64 41}
!95 = !DILocation(line: 65, column: 22, scope: !61)
!96 = !DILocation(line: 65, column: 18, scope: !61)
!97 = !DILocation(line: 65, column: 20, scope: !61)
!98 = !{!71, !34, i64 42}
!99 = !DILocation(line: 66, column: 1, scope: !61)
!100 = distinct !DISubprogram(name: "updateDot", scope: !9, file: !9, line: 69, type: !62, scopeLine: 69, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !101)
!101 = !{!102}
!102 = !DILocalVariable(name: "CurDot", arg: 1, scope: !100, file: !9, line: 69, type: !64)
!103 = !DILocation(line: 0, scope: !100)
!104 = !DILocation(line: 70, column: 15, scope: !105)
!105 = distinct !DILexicalBlock(scope: !100, file: !9, line: 70, column: 7)
!106 = !{!71, !72, i64 24}
!107 = !DILocation(line: 70, column: 31, scope: !105)
!108 = !DILocation(line: 70, column: 37, scope: !105)
!109 = !DILocation(line: 70, column: 61, scope: !105)
!110 = !{!71, !33, i64 16}
!111 = !DILocation(line: 70, column: 50, scope: !105)
!112 = !DILocation(line: 70, column: 7, scope: !100)
!113 = !DILocation(line: 71, column: 5, scope: !114)
!114 = distinct !DILexicalBlock(scope: !105, file: !9, line: 70, column: 74)
!115 = !DILocation(line: 72, column: 5, scope: !114)
!116 = !DILocation(line: 75, column: 15, scope: !117)
!117 = distinct !DILexicalBlock(scope: !100, file: !9, line: 75, column: 7)
!118 = !DILocation(line: 75, column: 17, scope: !117)
!119 = !DILocation(line: 75, column: 7, scope: !100)
!120 = !DILocation(line: 76, column: 29, scope: !117)
!121 = !DILocation(line: 76, column: 5, scope: !117)
!122 = !DILocation(line: 77, column: 46, scope: !100)
!123 = !DILocation(line: 77, column: 24, scope: !100)
!124 = !DILocation(line: 77, column: 36, scope: !100)
!125 = !DILocation(line: 77, column: 13, scope: !100)
!126 = !DILocation(line: 78, column: 1, scope: !100)
!127 = distinct !DISubprogram(name: "changeState", scope: !9, file: !9, line: 81, type: !62, scopeLine: 81, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !128)
!128 = !{!129, !130}
!129 = !DILocalVariable(name: "ArrayOfDots", arg: 1, scope: !127, file: !9, line: 81, type: !64)
!130 = !DILocalVariable(name: "i", scope: !131, file: !9, line: 82, type: !132)
!131 = distinct !DILexicalBlock(scope: !127, file: !9, line: 82, column: 3)
!132 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!133 = !DILocation(line: 0, scope: !127)
!134 = !DILocation(line: 0, scope: !131)
!135 = !DILocation(line: 82, column: 3, scope: !131)
!136 = !DILocation(line: 84, column: 1, scope: !127)
!137 = !DILocation(line: 83, column: 16, scope: !138)
!138 = distinct !DILexicalBlock(scope: !131, file: !9, line: 82, column: 3)
!139 = !DILocation(line: 83, column: 5, scope: !138)
!140 = !DILocation(line: 82, column: 33, scope: !138)
!141 = !DILocation(line: 82, column: 21, scope: !138)
!142 = distinct !{!142, !135, !143, !144, !145}
!143 = !DILocation(line: 83, column: 30, scope: !131)
!144 = !{!"llvm.loop.mustprogress"}
!145 = !{!"llvm.loop.unroll.disable"}
!146 = distinct !DISubprogram(name: "distance", scope: !9, file: !9, line: 87, type: !147, scopeLine: 87, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !149)
!147 = !DISubroutineType(types: !148)
!148 = !{!12, !12, !12, !12, !12}
!149 = !{!150, !151, !152, !153, !154, !155}
!150 = !DILocalVariable(name: "X1", arg: 1, scope: !146, file: !9, line: 87, type: !12)
!151 = !DILocalVariable(name: "Y1", arg: 2, scope: !146, file: !9, line: 87, type: !12)
!152 = !DILocalVariable(name: "X2", arg: 3, scope: !146, file: !9, line: 87, type: !12)
!153 = !DILocalVariable(name: "Y2", arg: 4, scope: !146, file: !9, line: 87, type: !12)
!154 = !DILocalVariable(name: "dX", scope: !146, file: !9, line: 88, type: !12)
!155 = !DILocalVariable(name: "dY", scope: !146, file: !9, line: 89, type: !12)
!156 = !DILocation(line: 0, scope: !146)
!157 = !DILocation(line: 88, column: 20, scope: !146)
!158 = !DILocation(line: 88, column: 17, scope: !146)
!159 = !DILocation(line: 89, column: 20, scope: !146)
!160 = !DILocation(line: 89, column: 17, scope: !146)
!161 = !DILocation(line: 90, column: 13, scope: !146)
!162 = !DILocation(line: 90, column: 23, scope: !146)
!163 = !DILocation(line: 90, column: 18, scope: !146)
!164 = !DILocation(line: 90, column: 3, scope: !146)
!165 = distinct !DISubprogram(name: "getNearestDot", scope: !9, file: !9, line: 94, type: !166, scopeLine: 95, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !168)
!166 = !DISubroutineType(types: !167)
!167 = !{!46, !12, !12, !46}
!168 = !{!169, !170, !171, !172, !173, !174, !176, !179}
!169 = !DILocalVariable(name: "X", arg: 1, scope: !165, file: !9, line: 94, type: !12)
!170 = !DILocalVariable(name: "Y", arg: 2, scope: !165, file: !9, line: 94, type: !12)
!171 = !DILocalVariable(name: "ArrayOfDots", arg: 3, scope: !165, file: !9, line: 95, type: !46)
!172 = !DILocalVariable(name: "NearestDot", scope: !165, file: !9, line: 96, type: !46)
!173 = !DILocalVariable(name: "MinDist", scope: !165, file: !9, line: 97, type: !12)
!174 = !DILocalVariable(name: "i", scope: !175, file: !9, line: 98, type: !12)
!175 = distinct !DILexicalBlock(scope: !165, file: !9, line: 98, column: 3)
!176 = !DILocalVariable(name: "CurDot", scope: !177, file: !9, line: 99, type: !46)
!177 = distinct !DILexicalBlock(scope: !178, file: !9, line: 98, column: 43)
!178 = distinct !DILexicalBlock(scope: !175, file: !9, line: 98, column: 3)
!179 = !DILocalVariable(name: "Dist", scope: !177, file: !9, line: 100, type: !12)
!180 = !DILocation(line: 0, scope: !165)
!181 = !DILocation(line: 0, scope: !175)
!182 = !DILocation(line: 98, column: 3, scope: !175)
!183 = !DILocation(line: 107, column: 3, scope: !165)
!184 = !DILocation(line: 99, column: 33, scope: !177)
!185 = !DILocation(line: 0, scope: !177)
!186 = !DILocation(line: 100, column: 44, scope: !177)
!187 = !DILocation(line: 100, column: 55, scope: !177)
!188 = !DILocation(line: 100, column: 21, scope: !177)
!189 = !DILocation(line: 102, column: 24, scope: !190)
!190 = distinct !DILexicalBlock(scope: !177, file: !9, line: 102, column: 9)
!191 = !DILocation(line: 102, column: 26, scope: !190)
!192 = !DILocation(line: 102, column: 14, scope: !190)
!193 = !DILocation(line: 102, column: 38, scope: !190)
!194 = !DILocation(line: 98, column: 38, scope: !178)
!195 = !DILocation(line: 98, column: 26, scope: !178)
!196 = distinct !{!196, !182, !197, !144, !145}
!197 = !DILocation(line: 106, column: 3, scope: !175)
!198 = distinct !DISubprogram(name: "drawFrame", scope: !9, file: !9, line: 111, type: !44, scopeLine: 111, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !199)
!199 = !{!200, !201, !203, !206}
!200 = !DILocalVariable(name: "ArrayOfDots", arg: 1, scope: !198, file: !9, line: 111, type: !46)
!201 = !DILocalVariable(name: "X", scope: !202, file: !9, line: 112, type: !12)
!202 = distinct !DILexicalBlock(scope: !198, file: !9, line: 112, column: 3)
!203 = !DILocalVariable(name: "Y", scope: !204, file: !9, line: 113, type: !12)
!204 = distinct !DILexicalBlock(scope: !205, file: !9, line: 113, column: 5)
!205 = distinct !DILexicalBlock(scope: !202, file: !9, line: 112, column: 3)
!206 = !DILocalVariable(name: "NearestDot", scope: !207, file: !9, line: 114, type: !46)
!207 = distinct !DILexicalBlock(scope: !208, file: !9, line: 113, column: 47)
!208 = distinct !DILexicalBlock(scope: !204, file: !9, line: 113, column: 5)
!209 = !DILocation(line: 0, scope: !198)
!210 = !DILocation(line: 0, scope: !202)
!211 = !DILocation(line: 112, column: 3, scope: !202)
!212 = !DILocation(line: 0, scope: !204)
!213 = !DILocation(line: 113, column: 5, scope: !204)
!214 = !DILocation(line: 121, column: 1, scope: !198)
!215 = !DILocation(line: 112, column: 40, scope: !205)
!216 = !DILocation(line: 112, column: 26, scope: !205)
!217 = distinct !{!217, !211, !218, !144, !145}
!218 = !DILocation(line: 120, column: 5, scope: !202)
!219 = !DILocation(line: 114, column: 38, scope: !207)
!220 = !DILocation(line: 0, scope: !207)
!221 = !DILocation(line: 115, column: 22, scope: !222)
!222 = distinct !DILexicalBlock(scope: !207, file: !9, line: 115, column: 11)
!223 = !DILocation(line: 115, column: 11, scope: !207)
!224 = !DILocation(line: 119, column: 7, scope: !207)
!225 = !DILocation(line: 120, column: 5, scope: !208)
!226 = !DILocation(line: 113, column: 42, scope: !208)
!227 = !DILocation(line: 113, column: 28, scope: !208)
!228 = distinct !{!228, !213, !229, !144, !145}
!229 = !DILocation(line: 120, column: 5, scope: !204)
!230 = !DISubprogram(name: "simPutPixel", scope: !19, file: !19, line: 13, type: !231, flags: DIFlagPrototyped, spFlags: DISPFlagOptimized, retainedNodes: !4)
!231 = !DISubroutineType(types: !232)
!232 = !{null, !132, !132, !18}
!233 = distinct !DISubprogram(name: "initDots", scope: !9, file: !9, line: 124, type: !62, scopeLine: 124, flags: DIFlagPrototyped | DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !234)
!234 = !{!235, !236}
!235 = !DILocalVariable(name: "ArrayOfDots", arg: 1, scope: !233, file: !9, line: 124, type: !64)
!236 = !DILocalVariable(name: "i", scope: !237, file: !9, line: 125, type: !132)
!237 = distinct !DILexicalBlock(scope: !233, file: !9, line: 125, column: 3)
!238 = !DILocation(line: 0, scope: !233)
!239 = !DILocation(line: 0, scope: !237)
!240 = !DILocation(line: 125, column: 3, scope: !237)
!241 = !DILocation(line: 127, column: 1, scope: !233)
!242 = !DILocation(line: 126, column: 21, scope: !243)
!243 = distinct !DILexicalBlock(scope: !237, file: !9, line: 125, column: 3)
!244 = !DILocation(line: 126, column: 5, scope: !243)
!245 = !DILocation(line: 125, column: 33, scope: !243)
!246 = !DILocation(line: 125, column: 21, scope: !243)
!247 = distinct !{!247, !240, !248, !144, !145}
!248 = !DILocation(line: 126, column: 35, scope: !237)
!249 = distinct !DISubprogram(name: "app", scope: !9, file: !9, line: 129, type: !250, scopeLine: 129, flags: DIFlagAllCallsDescribed, spFlags: DISPFlagDefinition | DISPFlagOptimized, unit: !2, retainedNodes: !252)
!250 = !DISubroutineType(types: !251)
!251 = !{null}
!252 = !{!253, !257}
!253 = !DILocalVariable(name: "ArrayOfDots", scope: !249, file: !9, line: 130, type: !254)
!254 = !DICompositeType(tag: DW_TAG_array_type, baseType: !48, size: 3840, elements: !255)
!255 = !{!256}
!256 = !DISubrange(count: 10)
!257 = !DILocalVariable(name: "i", scope: !258, file: !9, line: 132, type: !12)
!258 = distinct !DILexicalBlock(scope: !249, file: !9, line: 132, column: 3)
!259 = !DILocation(line: 130, column: 3, scope: !249)
!260 = !DILocation(line: 130, column: 14, scope: !249)
!261 = !DILocation(line: 131, column: 12, scope: !249)
!262 = !DILocation(line: 131, column: 3, scope: !249)
!263 = !DILocation(line: 0, scope: !258)
!264 = !DILocation(line: 133, column: 5, scope: !265)
!265 = distinct !DILexicalBlock(scope: !266, file: !9, line: 132, column: 36)
!266 = distinct !DILexicalBlock(scope: !258, file: !9, line: 132, column: 3)
!267 = !DILocation(line: 134, column: 5, scope: !265)
!268 = !DILocation(line: 135, column: 5, scope: !265)
!269 = !DILocation(line: 137, column: 1, scope: !249)
!270 = !DISubprogram(name: "simFlush", scope: !19, file: !19, line: 12, type: !271, spFlags: DISPFlagOptimized, retainedNodes: !4)
!271 = !DISubroutineType(types: !272)
!272 = !{null, null}
