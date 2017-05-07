; ModuleID = 'simpleFunc.sk'
source_filename = "simpleFunc.sk"

declare i32 @puts(i8*)

define i32 @main(i32 %argc, i8** %argv) {
  ret i32 0
}

define i32 @foo(i32 %y, i32) {
entry:
  ret i32 32
}
