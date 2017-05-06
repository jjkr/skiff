; ModuleID = 'simpleIf.sk'
source_filename = "simpleIf.sk"

declare i32 @puts(i8*)

define i32 @main(i32 %argc, i8** %argv) {
  br i1 true, label %then, label %else

then:                                             ; preds = %0
  br label %ifcont

else:                                             ; preds = %0
  br label %ifcont

ifcont:                                           ; preds = %else, %then
  %iftmp = phi i32 [ 9, %then ], [ 3, %else ]
                                                  ; No predecessors!
  ret i32 %iftmp
}
