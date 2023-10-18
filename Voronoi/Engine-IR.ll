!<arch>
Engine.c.o/     0           0     0     644     18289     `
; ModuleID = '/home/ii-sc/Education/LLVM/Voronoi/Engine.c'
source_filename = "/home/ii-sc/Education/LLVM/Voronoi/Engine.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.RGB = type { i8, i8, i8 }
%struct.Dot = type { i64, i64, i64, i32, i64, %struct.RGB }

@BackGround = dso_local local_unnamed_addr constant %struct.RGB { i8 10, i8 10, i8 10 }, align 1
@xorshift.Seed = internal unnamed_addr global i64 1, align 8

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn uwtable
define dso_local i64 @xorshift() local_unnamed_addr #0 {
  %1 = load i64, ptr @xorshift.Seed, align 8, !tbaa !5
  %2 = shl i64 %1, 13
  %3 = xor i64 %2, %1
  %4 = lshr i64 %3, 7
  %5 = xor i64 %4, %3
  %6 = shl i64 %5, 17
  %7 = xor i64 %6, %5
  store i64 %7, ptr @xorshift.Seed, align 8, !tbaa !5
  ret i64 %7
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind readnone willreturn uwtable
define dso_local void @dumpDots(ptr nocapture %0) local_unnamed_addr #1 {
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn uwtable
define dso_local void @changePosition(ptr nocapture noundef writeonly %0) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 4
  store i64 0, ptr %2, align 8, !tbaa !9
  %3 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 2
  store i64 5, ptr %3, align 8, !tbaa !13
  %4 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 3
  store i32 1, ptr %4, align 8, !tbaa !14
  %5 = load i64, ptr @xorshift.Seed, align 8, !tbaa !5
  %6 = shl i64 %5, 13
  %7 = xor i64 %6, %5
  %8 = lshr i64 %7, 7
  %9 = xor i64 %8, %7
  %10 = shl i64 %9, 17
  %11 = xor i64 %10, %9
  store i64 %11, ptr @xorshift.Seed, align 8, !tbaa !5
  %12 = and i64 %9, 511
  store i64 %12, ptr %0, align 8, !tbaa !15
  %13 = shl i64 %11, 13
  %14 = xor i64 %13, %11
  %15 = lshr i64 %14, 7
  %16 = xor i64 %15, %14
  %17 = shl i64 %16, 17
  %18 = xor i64 %17, %16
  store i64 %18, ptr @xorshift.Seed, align 8, !tbaa !5
  %19 = and i64 %16, 511
  %20 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 1
  store i64 %19, ptr %20, align 8, !tbaa !16
  %21 = shl i64 %18, 13
  %22 = xor i64 %21, %18
  %23 = lshr i64 %22, 7
  %24 = xor i64 %23, %22
  %25 = shl i64 %24, 17
  %26 = xor i64 %25, %24
  store i64 %26, ptr @xorshift.Seed, align 8, !tbaa !5
  %27 = trunc i64 %24 to i8
  %28 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 5
  store i8 %27, ptr %28, align 8, !tbaa !17
  %29 = shl i64 %26, 13
  %30 = xor i64 %29, %26
  %31 = lshr i64 %30, 7
  %32 = xor i64 %31, %30
  %33 = shl i64 %32, 17
  %34 = xor i64 %33, %32
  store i64 %34, ptr @xorshift.Seed, align 8, !tbaa !5
  %35 = trunc i64 %32 to i8
  %36 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 5, i32 1
  store i8 %35, ptr %36, align 1, !tbaa !18
  %37 = shl i64 %34, 13
  %38 = xor i64 %37, %34
  %39 = lshr i64 %38, 7
  %40 = xor i64 %39, %38
  %41 = shl i64 %40, 17
  %42 = xor i64 %41, %40
  store i64 %42, ptr @xorshift.Seed, align 8, !tbaa !5
  %43 = trunc i64 %40 to i8
  %44 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 5, i32 2
  store i8 %43, ptr %44, align 2, !tbaa !19
  ret void
}

; Function Attrs: mustprogress nofree norecurse nosync nounwind willreturn uwtable
define dso_local void @updateDot(ptr nocapture noundef %0) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 3
  %3 = load i32, ptr %2, align 8, !tbaa !14
  %4 = icmp eq i32 %3, -1
  br i1 %4, label %5, label %52

5:                                                ; preds = %1
  %6 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 4
  %7 = load i64, ptr %6, align 8, !tbaa !9
  %8 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 2
  %9 = load i64, ptr %8, align 8, !tbaa !13
  %10 = icmp ugt i64 %7, %9
  br i1 %10, label %52, label %11

11:                                               ; preds = %5
  store i64 0, ptr %6, align 8, !tbaa !9
  store i64 5, ptr %8, align 8, !tbaa !13
  store i32 1, ptr %2, align 8, !tbaa !14
  %12 = load i64, ptr @xorshift.Seed, align 8, !tbaa !5
  %13 = shl i64 %12, 13
  %14 = xor i64 %13, %12
  %15 = lshr i64 %14, 7
  %16 = xor i64 %15, %14
  %17 = shl i64 %16, 17
  %18 = xor i64 %17, %16
  store i64 %18, ptr @xorshift.Seed, align 8, !tbaa !5
  %19 = and i64 %16, 511
  store i64 %19, ptr %0, align 8, !tbaa !15
  %20 = shl i64 %18, 13
  %21 = xor i64 %20, %18
  %22 = lshr i64 %21, 7
  %23 = xor i64 %22, %21
  %24 = shl i64 %23, 17
  %25 = xor i64 %24, %23
  store i64 %25, ptr @xorshift.Seed, align 8, !tbaa !5
  %26 = and i64 %23, 511
  %27 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 1
  store i64 %26, ptr %27, align 8, !tbaa !16
  %28 = shl i64 %25, 13
  %29 = xor i64 %28, %25
  %30 = lshr i64 %29, 7
  %31 = xor i64 %30, %29
  %32 = shl i64 %31, 17
  %33 = xor i64 %32, %31
  store i64 %33, ptr @xorshift.Seed, align 8, !tbaa !5
  %34 = trunc i64 %31 to i8
  %35 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 5
  store i8 %34, ptr %35, align 8, !tbaa !17
  %36 = shl i64 %33, 13
  %37 = xor i64 %36, %33
  %38 = lshr i64 %37, 7
  %39 = xor i64 %38, %37
  %40 = shl i64 %39, 17
  %41 = xor i64 %40, %39
  store i64 %41, ptr @xorshift.Seed, align 8, !tbaa !5
  %42 = trunc i64 %39 to i8
  %43 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 5, i32 1
  store i8 %42, ptr %43, align 1, !tbaa !18
  %44 = shl i64 %41, 13
  %45 = xor i64 %44, %41
  %46 = lshr i64 %45, 7
  %47 = xor i64 %46, %45
  %48 = shl i64 %47, 17
  %49 = xor i64 %48, %47
  store i64 %49, ptr @xorshift.Seed, align 8, !tbaa !5
  %50 = trunc i64 %47 to i8
  %51 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 5, i32 2
  store i8 %50, ptr %51, align 2, !tbaa !19
  br label %64

52:                                               ; preds = %5, %1
  %53 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 4
  %54 = load i64, ptr %53, align 8, !tbaa !9
  %55 = icmp ugt i64 %54, 512
  br i1 %55, label %56, label %57

56:                                               ; preds = %52
  store i32 -1, ptr %2, align 8, !tbaa !14
  br label %57

57:                                               ; preds = %56, %52
  %58 = getelementptr inbounds %struct.Dot, ptr %0, i64 0, i32 2
  %59 = load i64, ptr %58, align 8, !tbaa !13
  %60 = load i32, ptr %2, align 8, !tbaa !14
  %61 = sext i32 %60 to i64
  %62 = mul i64 %59, %61
  %63 = add i64 %62, %54
  store i64 %63, ptr %53, align 8, !tbaa !9
  br label %64

64:                                               ; preds = %57, %11
  ret void
}

; Function Attrs: nofree norecurse nosync nounwind uwtable
define dso_local void @changeState(ptr nocapture noundef %0) local_unnamed_addr #2 {
  br label %3

2:                                                ; preds = %3
  ret void

3:                                                ; preds = %1, %3
  %4 = phi i64 [ 0, %1 ], [ %6, %3 ]
  %5 = getelementptr inbounds %struct.Dot, ptr %0, i64 %4
  tail call void @updateDot(ptr noundef %5)
  %6 = add nuw nsw i64 %4, 1
  %7 = icmp eq i64 %6, 10
  br i1 %7, label %2, label %3, !llvm.loop !20
}

; Function Attrs: argmemonly mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #3

; Function Attrs: argmemonly mustprogress nocallback nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #3

; Function Attrs: mustprogress nofree norecurse nosync nounwind readnone willreturn uwtable
define dso_local i64 @distance(i64 noundef %0, i64 noundef %1, i64 noundef %2, i64 noundef %3) local_unnamed_addr #1 {
  %5 = icmp ugt i64 %2, %0
  %6 = sub i64 %2, %0
  %7 = sub i64 %0, %2
  %8 = select i1 %5, i64 %6, i64 %7
  %9 = icmp ugt i64 %3, %1
  %10 = sub i64 %3, %1
  %11 = sub i64 %1, %3
  %12 = select i1 %9, i64 %10, i64 %11
  %13 = mul i64 %8, %8
  %14 = mul i64 %12, %12
  %15 = add i64 %14, %13
  ret i64 %15
}

; Function Attrs: argmemonly nofree norecurse nosync nounwind readonly uwtable
define dso_local ptr @getNearestDot(i64 noundef %0, i64 noundef %1, ptr noundef readonly %2) local_unnamed_addr #4 {
  br label %5

4:                                                ; preds = %5
  ret ptr %31

5:                                                ; preds = %3, %5
  %6 = phi ptr [ null, %3 ], [ %31, %5 ]
  %7 = phi i64 [ 48830, %3 ], [ %30, %5 ]
  %8 = phi i64 [ 0, %3 ], [ %32, %5 ]
  %9 = getelementptr inbounds %struct.Dot, ptr %2, i64 %8
  %10 = load i64, ptr %9, align 8, !tbaa !15
  %11 = getelementptr inbounds %struct.Dot, ptr %2, i64 %8, i32 1
  %12 = load i64, ptr %11, align 8, !tbaa !16
  %13 = icmp ugt i64 %10, %0
  %14 = sub i64 %10, %0
  %15 = sub i64 %0, %10
  %16 = select i1 %13, i64 %14, i64 %15
  %17 = icmp ugt i64 %12, %1
  %18 = sub i64 %12, %1
  %19 = sub i64 %1, %12
  %20 = select i1 %17, i64 %18, i64 %19
  %21 = mul i64 %16, %16
  %22 = mul i64 %20, %20
  %23 = add i64 %22, %21
  %24 = getelementptr inbounds %struct.Dot, ptr %2, i64 %8, i32 4
  %25 = load i64, ptr %24, align 8, !tbaa !9
  %26 = mul i64 %25, %25
  %27 = icmp ult i64 %23, %26
  %28 = icmp ult i64 %23, %7
  %29 = select i1 %27, i1 %28, i1 false
  %30 = select i1 %29, i64 %23, i64 %7
  %31 = select i1 %29, ptr %9, ptr %6
  %32 = add nuw nsw i64 %8, 1
  %33 = icmp eq i64 %32, 10
  br i1 %33, label %4, label %5, !llvm.loop !23
}

; Function Attrs: nounwind uwtable
define dso_local void @drawFrame(ptr noundef readonly %0) local_unnamed_addr #5 {
  br label %2

2:                                                ; preds = %1, %9
  %3 = phi i64 [ 0, %1 ], [ %10, %9 ]
  %4 = trunc i64 %3 to i32
  %5 = trunc i64 %3 to i32
  br label %7

6:                                                ; preds = %9
  ret void

7:                                                ; preds = %2, %48
  %8 = phi i64 [ 0, %2 ], [ %49, %48 ]
  br label %12

9:                                                ; preds = %48
  %10 = add nuw nsw i64 %3, 1
  %11 = icmp eq i64 %10, 512
  br i1 %11, label %6, label %2, !llvm.loop !24

12:                                               ; preds = %7, %12
  %13 = phi ptr [ %38, %12 ], [ null, %7 ]
  %14 = phi i64 [ %37, %12 ], [ 48830, %7 ]
  %15 = phi i64 [ %39, %12 ], [ 0, %7 ]
  %16 = getelementptr inbounds %struct.Dot, ptr %0, i64 %15
  %17 = load i64, ptr %16, align 8, !tbaa !15
  %18 = getelementptr inbounds %struct.Dot, ptr %0, i64 %15, i32 1
  %19 = load i64, ptr %18, align 8, !tbaa !16
  %20 = icmp ugt i64 %17, %3
  %21 = sub i64 %17, %3
  %22 = sub i64 %3, %17
  %23 = select i1 %20, i64 %21, i64 %22
  %24 = icmp ugt i64 %19, %8
  %25 = sub i64 %19, %8
  %26 = sub i64 %8, %19
  %27 = select i1 %24, i64 %25, i64 %26
  %28 = mul i64 %23, %23
  %29 = mul i64 %27, %27
  %30 = add i64 %29, %28
  %31 = getelementptr inbounds %struct.Dot, ptr %0, i64 %15, i32 4
  %32 = load i64, ptr %31, align 8, !tbaa !9
  %33 = mul i64 %32, %32
  %34 = icmp ult i64 %30, %33
  %35 = icmp ult i64 %30, %14
  %36 = select i1 %34, i1 %35, i1 false
  %37 = select i1 %36, i64 %30, i64 %14
  %38 = select i1 %36, ptr %16, ptr %13
  %39 = add nuw nsw i64 %15, 1
  %40 = icmp eq i64 %39, 10
  br i1 %40, label %41, label %12, !llvm.loop !23

41:                                               ; preds = %12
  %42 = icmp eq ptr %38, null
  %43 = trunc i64 %8 to i32
  br i1 %42, label %44, label %45

44:                                               ; preds = %41
  tail call void @simPutPixel(i32 noundef %5, i32 noundef %43, i24 657930) #7
  br label %48

45:                                               ; preds = %41
  %46 = getelementptr inbounds %struct.Dot, ptr %38, i64 0, i32 5
  %47 = load i24, ptr %46, align 8
  tail call void @simPutPixel(i32 noundef %4, i32 noundef %43, i24 %47) #7
  br label %48

48:                                               ; preds = %45, %44
  %49 = add nuw nsw i64 %8, 1
  %50 = icmp eq i64 %49, 512
  br i1 %50, label %9, label %7, !llvm.loop !25
}

declare void @simPutPixel(i32 noundef, i32 noundef, i24) local_unnamed_addr #6

; Function Attrs: nofree norecurse nosync nounwind uwtable
define dso_local void @initDots(ptr nocapture noundef writeonly %0) local_unnamed_addr #2 {
  %2 = load i64, ptr @xorshift.Seed, align 8, !tbaa !5
  br label %4

3:                                                ; preds = %4
  store i64 %47, ptr @xorshift.Seed, align 8, !tbaa !5
  ret void

4:                                                ; preds = %1, %4
  %5 = phi i64 [ 0, %1 ], [ %50, %4 ]
  %6 = phi i64 [ %2, %1 ], [ %47, %4 ]
  %7 = getelementptr inbounds %struct.Dot, ptr %0, i64 %5
  %8 = getelementptr inbounds %struct.Dot, ptr %0, i64 %5, i32 4
  store i64 0, ptr %8, align 8, !tbaa !9
  %9 = getelementptr inbounds %struct.Dot, ptr %0, i64 %5, i32 2
  store i64 5, ptr %9, align 8, !tbaa !13
  %10 = getelementptr inbounds %struct.Dot, ptr %0, i64 %5, i32 3
  store i32 1, ptr %10, align 8, !tbaa !14
  %11 = shl i64 %6, 13
  %12 = xor i64 %11, %6
  %13 = lshr i64 %12, 7
  %14 = xor i64 %13, %12
  %15 = shl i64 %14, 17
  %16 = xor i64 %15, %14
  %17 = and i64 %14, 511
  store i64 %17, ptr %7, align 8, !tbaa !15
  %18 = shl i64 %16, 13
  %19 = xor i64 %18, %16
  %20 = lshr i64 %19, 7
  %21 = xor i64 %20, %19
  %22 = shl i64 %21, 17
  %23 = xor i64 %22, %21
  %24 = and i64 %21, 511
  %25 = getelementptr inbounds %struct.Dot, ptr %0, i64 %5, i32 1
  store i64 %24, ptr %25, align 8, !tbaa !16
  %26 = shl i64 %23, 13
  %27 = xor i64 %26, %23
  %28 = lshr i64 %27, 7
  %29 = xor i64 %28, %27
  %30 = shl i64 %29, 17
  %31 = xor i64 %30, %29
  %32 = trunc i64 %29 to i8
  %33 = getelementptr inbounds %struct.Dot, ptr %0, i64 %5, i32 5
  store i8 %32, ptr %33, align 8, !tbaa !17
  %34 = shl i64 %31, 13
  %35 = xor i64 %34, %31
  %36 = lshr i64 %35, 7
  %37 = xor i64 %36, %35
  %38 = shl i64 %37, 17
  %39 = xor i64 %38, %37
  %40 = trunc i64 %37 to i8
  %41 = getelementptr inbounds %struct.Dot, ptr %0, i64 %5, i32 5, i32 1
  store i8 %40, ptr %41, align 1, !tbaa !18
  %42 = shl i64 %39, 13
  %43 = xor i64 %42, %39
  %44 = lshr i64 %43, 7
  %45 = xor i64 %44, %43
  %46 = shl i64 %45, 17
  %47 = xor i64 %46, %45
  %48 = trunc i64 %45 to i8
  %49 = getelementptr inbounds %struct.Dot, ptr %0, i64 %5, i32 5, i32 2
  store i8 %48, ptr %49, align 2, !tbaa !19
  %50 = add nuw nsw i64 %5, 1
  %51 = icmp eq i64 %50, 10
  br i1 %51, label %3, label %4, !llvm.loop !26
}

; Function Attrs: nounwind uwtable
define dso_local void @app() local_unnamed_addr #5 {
  %1 = alloca [10 x %struct.Dot], align 16
  call void @llvm.lifetime.start.p0(i64 480, ptr nonnull %1) #7
  call void @initDots(ptr noundef nonnull %1)
  br label %2

2:                                                ; preds = %0, %10
  %3 = phi i64 [ 0, %0 ], [ %11, %10 ]
  br label %5

4:                                                ; preds = %10
  call void @llvm.lifetime.end.p0(i64 480, ptr nonnull %1) #7
  ret void

5:                                                ; preds = %2, %5
  %6 = phi i64 [ %8, %5 ], [ 0, %2 ]
  %7 = getelementptr inbounds %struct.Dot, ptr %1, i64 %6
  call void @updateDot(ptr noundef nonnull %7)
  %8 = add nuw nsw i64 %6, 1
  %9 = icmp eq i64 %8, 10
  br i1 %9, label %10, label %5, !llvm.loop !20

10:                                               ; preds = %5
  call void @drawFrame(ptr noundef nonnull %1)
  call void (...) @simFlush() #7
  %11 = add nuw nsw i64 %3, 1
  %12 = icmp eq i64 %11, 10090
  br i1 %12, label %4, label %2, !llvm.loop !27
}

declare void @simFlush(...) local_unnamed_addr #6

attributes #0 = { mustprogress nofree norecurse nosync nounwind willreturn uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress nofree norecurse nosync nounwind readnone willreturn uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nofree norecurse nosync nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #3 = { argmemonly mustprogress nocallback nofree nosync nounwind willreturn }
attributes #4 = { argmemonly nofree norecurse nosync nounwind readonly uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nounwind uwtable "frame-pointer"="none" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { "frame-pointer"="none" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"Ubuntu clang version 15.0.7"}
!5 = !{!6, !6, i64 0}
!6 = !{!"long", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C/C++ TBAA"}
!9 = !{!10, !6, i64 32}
!10 = !{!"Dot", !6, i64 0, !6, i64 8, !6, i64 16, !11, i64 24, !6, i64 32, !12, i64 40}
!11 = !{!"int", !7, i64 0}
!12 = !{!"RGB", !7, i64 0, !7, i64 1, !7, i64 2}
!13 = !{!10, !6, i64 16}
!14 = !{!10, !11, i64 24}
!15 = !{!10, !6, i64 0}
!16 = !{!10, !6, i64 8}
!17 = !{!10, !7, i64 40}
!18 = !{!10, !7, i64 41}
!19 = !{!10, !7, i64 42}
!20 = distinct !{!20, !21, !22}
!21 = !{!"llvm.loop.mustprogress"}
!22 = !{!"llvm.loop.unroll.disable"}
!23 = distinct !{!23, !21, !22}
!24 = distinct !{!24, !21, !22}
!25 = distinct !{!25, !21, !22}
!26 = distinct !{!26, !21, !22}
!27 = distinct !{!27, !21, !22}

