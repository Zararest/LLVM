!<arch>
Engine.c.o/     0           0     0     644     14331     `
; ModuleID = '/home/ii-sc/Education/LLVM/Voronoi/Engine.c'
source_filename = "/home/ii-sc/Education/LLVM/Voronoi/Engine.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

%struct.RGB = type { i8, i8, i8 }
%struct.Dot = type { i64, i64, i64, i32, i64, %struct.RGB }

@BackGround = dso_local local_unnamed_addr constant %struct.RGB { i8 10, i8 10, i8 10 }, align 1
@xorshift.Seed = internal unnamed_addr global i64 1, align 8

; Function Attrs: nofree noinline norecurse nounwind uwtable willreturn
define dso_local i64 @xorshift() local_unnamed_addr #0 {
  %1 = load i64, i64* @xorshift.Seed, align 8, !tbaa !2
  %2 = shl i64 %1, 13
  %3 = xor i64 %2, %1
  %4 = lshr i64 %3, 7
  %5 = xor i64 %4, %3
  %6 = shl i64 %5, 17
  %7 = xor i64 %6, %5
  store i64 %7, i64* @xorshift.Seed, align 8, !tbaa !2
  ret i64 %7
}

; Function Attrs: norecurse nounwind readnone uwtable willreturn
define dso_local void @dumpDots(%struct.Dot* nocapture %0) local_unnamed_addr #1 {
  ret void
}

; Function Attrs: nofree noinline norecurse nounwind uwtable willreturn
define dso_local void @changePosition(%struct.Dot* nocapture %0) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 4
  store i64 0, i64* %2, align 8, !tbaa !6
  %3 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 2
  store i64 5, i64* %3, align 8, !tbaa !10
  %4 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 3
  store i32 1, i32* %4, align 8, !tbaa !11
  %5 = tail call i64 @xorshift()
  %6 = and i64 %5, 63
  %7 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 0
  store i64 %6, i64* %7, align 8, !tbaa !12
  %8 = tail call i64 @xorshift()
  %9 = and i64 %8, 63
  %10 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 1
  store i64 %9, i64* %10, align 8, !tbaa !13
  %11 = tail call i64 @xorshift()
  %12 = trunc i64 %11 to i8
  %13 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 5, i32 0
  store i8 %12, i8* %13, align 8, !tbaa !14
  %14 = tail call i64 @xorshift()
  %15 = trunc i64 %14 to i8
  %16 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 5, i32 1
  store i8 %15, i8* %16, align 1, !tbaa !15
  %17 = tail call i64 @xorshift()
  %18 = trunc i64 %17 to i8
  %19 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 5, i32 2
  store i8 %18, i8* %19, align 2, !tbaa !16
  ret void
}

; Function Attrs: nofree noinline norecurse nounwind uwtable willreturn
define dso_local void @updateDot(%struct.Dot* nocapture %0) local_unnamed_addr #0 {
  %2 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 3
  %3 = load i32, i32* %2, align 8, !tbaa !11
  %4 = icmp eq i32 %3, -1
  %5 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 4
  %6 = load i64, i64* %5, align 8, !tbaa !6
  br i1 %4, label %7, label %12

7:                                                ; preds = %1
  %8 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 2
  %9 = load i64, i64* %8, align 8, !tbaa !10
  %10 = icmp ugt i64 %6, %9
  br i1 %10, label %12, label %11

11:                                               ; preds = %7
  tail call void @changePosition(%struct.Dot* nonnull %0)
  br label %23

12:                                               ; preds = %1, %7
  %13 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 4
  %14 = icmp ugt i64 %6, 64
  br i1 %14, label %15, label %16

15:                                               ; preds = %12
  store i32 -1, i32* %2, align 8, !tbaa !11
  br label %16

16:                                               ; preds = %15, %12
  %17 = phi i32 [ -1, %15 ], [ %3, %12 ]
  %18 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 0, i32 2
  %19 = load i64, i64* %18, align 8, !tbaa !10
  %20 = sext i32 %17 to i64
  %21 = mul i64 %19, %20
  %22 = add i64 %21, %6
  store i64 %22, i64* %13, align 8, !tbaa !6
  br label %23

23:                                               ; preds = %16, %11
  ret void
}

; Function Attrs: nofree noinline norecurse nounwind uwtable
define dso_local void @changeState(%struct.Dot* nocapture %0) local_unnamed_addr #2 {
  br label %3

2:                                                ; preds = %3
  ret void

3:                                                ; preds = %1, %3
  %4 = phi i64 [ 0, %1 ], [ %6, %3 ]
  %5 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 %4
  tail call void @updateDot(%struct.Dot* %5)
  %6 = add nuw nsw i64 %4, 1
  %7 = icmp eq i64 %6, 10
  br i1 %7, label %2, label %3, !llvm.loop !17
}

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg, i8* nocapture) #3

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0i8(i64 immarg, i8* nocapture) #3

; Function Attrs: noinline norecurse nounwind readnone uwtable willreturn
define dso_local i64 @distance(i64 %0, i64 %1, i64 %2, i64 %3) local_unnamed_addr #4 {
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

; Function Attrs: noinline norecurse nounwind readonly uwtable
define dso_local %struct.Dot* @getNearestDot(i64 %0, i64 %1, %struct.Dot* readonly %2) local_unnamed_addr #5 {
  br label %5

4:                                                ; preds = %5
  ret %struct.Dot* %22

5:                                                ; preds = %3, %5
  %6 = phi %struct.Dot* [ null, %3 ], [ %22, %5 ]
  %7 = phi i64 [ 48830, %3 ], [ %21, %5 ]
  %8 = phi i64 [ 0, %3 ], [ %23, %5 ]
  %9 = getelementptr inbounds %struct.Dot, %struct.Dot* %2, i64 %8
  %10 = getelementptr inbounds %struct.Dot, %struct.Dot* %9, i64 0, i32 0
  %11 = load i64, i64* %10, align 8, !tbaa !12
  %12 = getelementptr inbounds %struct.Dot, %struct.Dot* %2, i64 %8, i32 1
  %13 = load i64, i64* %12, align 8, !tbaa !13
  %14 = tail call i64 @distance(i64 %0, i64 %1, i64 %11, i64 %13)
  %15 = getelementptr inbounds %struct.Dot, %struct.Dot* %2, i64 %8, i32 4
  %16 = load i64, i64* %15, align 8, !tbaa !6
  %17 = mul i64 %16, %16
  %18 = icmp ult i64 %14, %17
  %19 = icmp ult i64 %14, %7
  %20 = and i1 %19, %18
  %21 = select i1 %20, i64 %14, i64 %7
  %22 = select i1 %20, %struct.Dot* %9, %struct.Dot* %6
  %23 = add nuw nsw i64 %8, 1
  %24 = icmp eq i64 %23, 10
  br i1 %24, label %4, label %5, !llvm.loop !20
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @drawFrame(%struct.Dot* readonly %0) local_unnamed_addr #6 {
  br label %2

2:                                                ; preds = %1, %6
  %3 = phi i64 [ 0, %1 ], [ %7, %6 ]
  %4 = trunc i64 %3 to i32
  br label %9

5:                                                ; preds = %6
  ret void

6:                                                ; preds = %18
  %7 = add nuw nsw i64 %3, 1
  %8 = icmp eq i64 %7, 64
  br i1 %8, label %5, label %2, !llvm.loop !21

9:                                                ; preds = %2, %18
  %10 = phi i64 [ 0, %2 ], [ %20, %18 ]
  %11 = tail call %struct.Dot* @getNearestDot(i64 %3, i64 %10, %struct.Dot* %0)
  %12 = icmp eq %struct.Dot* %11, null
  %13 = trunc i64 %10 to i32
  br i1 %12, label %18, label %14

14:                                               ; preds = %9
  %15 = getelementptr inbounds %struct.Dot, %struct.Dot* %11, i64 0, i32 5
  %16 = bitcast %struct.RGB* %15 to i24*
  %17 = load i24, i24* %16, align 8
  br label %18

18:                                               ; preds = %9, %14
  %19 = phi i24 [ %17, %14 ], [ 657930, %9 ]
  tail call void @simPutPixel(i32 %4, i32 %13, i24 %19) #9
  %20 = add nuw nsw i64 %10, 1
  %21 = icmp eq i64 %20, 64
  br i1 %21, label %6, label %9, !llvm.loop !22
}

declare dso_local void @simPutPixel(i32, i32, i24) local_unnamed_addr #7

; Function Attrs: nofree noinline norecurse nounwind uwtable
define dso_local void @initDots(%struct.Dot* nocapture %0) local_unnamed_addr #2 {
  br label %3

2:                                                ; preds = %3
  ret void

3:                                                ; preds = %1, %3
  %4 = phi i64 [ 0, %1 ], [ %6, %3 ]
  %5 = getelementptr inbounds %struct.Dot, %struct.Dot* %0, i64 %4
  tail call void @changePosition(%struct.Dot* %5)
  %6 = add nuw nsw i64 %4, 1
  %7 = icmp eq i64 %6, 10
  br i1 %7, label %2, label %3, !llvm.loop !23
}

; Function Attrs: nounwind uwtable
define dso_local void @app() local_unnamed_addr #8 {
  %1 = alloca [10 x %struct.Dot], align 16
  %2 = bitcast [10 x %struct.Dot]* %1 to i8*
  call void @llvm.lifetime.start.p0i8(i64 480, i8* nonnull %2) #9
  %3 = getelementptr inbounds [10 x %struct.Dot], [10 x %struct.Dot]* %1, i64 0, i64 0
  call void @initDots(%struct.Dot* nonnull %3)
  br label %5

4:                                                ; preds = %5
  call void @llvm.lifetime.end.p0i8(i64 480, i8* nonnull %2) #9
  ret void

5:                                                ; preds = %0, %5
  %6 = phi i64 [ 0, %0 ], [ %7, %5 ]
  call void @changeState(%struct.Dot* nonnull %3)
  call void @drawFrame(%struct.Dot* nonnull %3)
  call void (...) @simFlush() #9
  %7 = add nuw nsw i64 %6, 1
  %8 = icmp eq i64 %7, 100
  br i1 %8, label %4, label %5, !llvm.loop !24
}

declare dso_local void @simFlush(...) local_unnamed_addr #7

attributes #0 = { nofree noinline norecurse nounwind uwtable willreturn "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { norecurse nounwind readnone uwtable willreturn "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nofree noinline norecurse nounwind uwtable "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { argmemonly nofree nosync nounwind willreturn }
attributes #4 = { noinline norecurse nounwind readnone uwtable willreturn "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { noinline norecurse nounwind readonly uwtable "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #6 = { noinline nounwind uwtable "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #7 = { "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #8 = { nounwind uwtable "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #9 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"Ubuntu clang version 12.0.1-19ubuntu3"}
!2 = !{!3, !3, i64 0}
!3 = !{!"long", !4, i64 0}
!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!6 = !{!7, !3, i64 32}
!7 = !{!"Dot", !3, i64 0, !3, i64 8, !3, i64 16, !8, i64 24, !3, i64 32, !9, i64 40}
!8 = !{!"int", !4, i64 0}
!9 = !{!"RGB", !4, i64 0, !4, i64 1, !4, i64 2}
!10 = !{!7, !3, i64 16}
!11 = !{!7, !8, i64 24}
!12 = !{!7, !3, i64 0}
!13 = !{!7, !3, i64 8}
!14 = !{!7, !4, i64 40}
!15 = !{!7, !4, i64 41}
!16 = !{!7, !4, i64 42}
!17 = distinct !{!17, !18, !19}
!18 = !{!"llvm.loop.mustprogress"}
!19 = !{!"llvm.loop.unroll.disable"}
!20 = distinct !{!20, !18, !19}
!21 = distinct !{!21, !18, !19}
!22 = distinct !{!22, !18, !19}
!23 = distinct !{!23, !18, !19}
!24 = distinct !{!24, !18, !19}

