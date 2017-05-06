; ModuleID = 'recursion.sk'
source_filename = "recursion.sk"

declare i32 @puts(i8*)

define i32 @main(i32 %argc, i8** %argv) {
  %1 = call i32 @recurse(i32 5)
  ret i32 %1
}

define i32 @recurse(i32 %x) {
entry:
  %0 = call i32 @recurse(i32 %x)
  ret i32 %0
}
