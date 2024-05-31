@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define dso_local i32 @main() local_unnamed_addr #1 {
  br label %2

1:                                                ; preds = %2
  ret i32 0

2:                                                ; preds = %0, %2
  %3 = phi i32 [ 0, %0 ], [ %5, %2 ]
  %4 = tail call i32 (ptr, ...) @printf(ptr noundef nonnull dereferenceable(1) @.str, i32 noundef 13)
  %5 = add nuw nsw i32 %3, 1
  %6 = icmp eq i32 %5, 4
  br i1 %6, label %1, label %2
}

declare noundef i32 @printf(ptr nocapture noundef readonly, ...) local_unnamed_addr #2