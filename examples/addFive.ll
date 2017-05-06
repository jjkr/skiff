; ModuleID = 'addFive.sk'
source_filename = "addFive.sk"

@0 = common constant [6 x i8] c"hello\00"

declare i32 @puts(i8*)

define i32 @main(i32 %argc, i8** %argv) {
  %1 = call i32 @id([6 x i8]* @0)
  %2 = call i32 @addFive(i32 8)
  ret i32 %2
}

define i32 @id(i32 %x) {
entry:
  ret i32 %x
}

define i32 @addFive(i32 %x) {
entry:
  %0 = add i32 %x, 2
  %1 = add i32 %0, 3
  ret i32 %1
}
