; ModuleID = 'mulAdd.sk'
source_filename = "mulAdd.sk"

declare i32 @puts(i8*)

define i32 @main(i32 %argc, i8** %argv) {
  ret i32 0
}

define i32 @mulAdd(i32 %x, i32 %y, i32 %z) {
entry:
  %0 = mul i32 %x, %y
  %1 = add i32 %0, %z
  ret i32 %1
}
