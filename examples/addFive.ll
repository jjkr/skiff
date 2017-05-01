; ModuleID = 'addFive.sk'
source_filename = "addFive.sk"

define i32 @moduleMain(i32 %argc, i8** %argv) {
  ret i32 0
}

define i32 @addFive(i32 %x) {
entry:
  %0 = add i32 %x, 2
  %1 = add i32 %0, 3
  ret i32 %1
}
