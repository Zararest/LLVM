define i32 @foo(i32 %x, i32 %y) #0 {
entry:
%add = add nsw i32 %x, %y
ret i32 1
}