; ModuleID = 'addFive.sk'
source_filename = "addFive.sk"

define i32 @main(i32 %argc, i8** %argv) {
  ret i32 8
}

define i32 @addFive(i32 %x) {
entry:
  %0 = add i32 %x, 2
  %1 = add i32 %0, 3
  ret i32 %1
  %2 = call i32 @addFive(i32 8)
}
