# skiff
A new language for the web

### Clang and LLVM

##### Links
 * [LLVM Tutorial](http://llvm.org/docs/tutorial/index.html)
 * [LLVM Language Manual](http://llvm.org/docs/LangRef.html)
 * [LLVM Programmer's Manual](http://llvm.org/docs/ProgrammersManual.html)
 * [LLVM doxygen](http://llvm.org/docs/doxygen/html/index.html)

##### Notes
Output clang AST
```
$ clang -Xclang -ast-dump -fsyntax-only foo.c
```

```
TranslationUnitDecl 0x7fdb73823cd0 <<invalid sloc>> <invalid sloc>
|-TypedefDecl 0x7fdb73824218 <<invalid sloc>> <invalid sloc> implicit __int128_t '__int128'
| `-BuiltinType 0x7fdb73823f40 '__int128'
|-TypedefDecl 0x7fdb73824278 <<invalid sloc>> <invalid sloc> implicit __uint128_t 'unsigned __int128'
| `-BuiltinType 0x7fdb73823f60 'unsigned __int128'
|-TypedefDecl 0x7fdb73824548 <<invalid sloc>> <invalid sloc> implicit __NSConstantString 'struct __NSConstantString_tag'
| `-RecordType 0x7fdb73824350 'struct __NSConstantString_tag'
|   `-Record 0x7fdb738242c8 '__NSConstantString_tag'
|-TypedefDecl 0x7fdb738245d8 <<invalid sloc>> <invalid sloc> implicit __builtin_ms_va_list 'char *'
| `-PointerType 0x7fdb738245a0 'char *'
|   `-BuiltinType 0x7fdb73823d60 'char'
|-TypedefDecl 0x7fdb73824898 <<invalid sloc>> <invalid sloc> implicit __builtin_va_list 'struct __va_list_tag [1]'
| `-ConstantArrayType 0x7fdb73824840 'struct __va_list_tag [1]' 1
|   `-RecordType 0x7fdb738246b0 'struct __va_list_tag'
|     `-Record 0x7fdb73824628 '__va_list_tag'
|-RecordDecl 0x7fdb738248e8 <hello.c:1:1, line:5:1> line:1:8 struct Pair definition
| |-FieldDecl 0x7fdb738249a0 <line:3:5, col:9> col:9 x 'int'
| `-FieldDecl 0x7fdb73876a00 <line:4:5, col:9> col:9 y 'int'
|-FunctionDecl 0x7fdb73876b20 <line:7:1, line:11:1> line:7:5 used addFive 'int (int)'
| |-ParmVarDecl 0x7fdb73876a58 <col:13, col:17> col:17 used x 'int'
| `-CompoundStmt 0x7fdb73876d78 <line:8:1, line:11:1>
|   |-DeclStmt 0x7fdb73876d08 <line:9:5, col:23>
|   | `-VarDecl 0x7fdb73876c20 <col:5, col:22> col:9 result 'int' cinit
|   |   `-BinaryOperator 0x7fdb73876ce0 <col:18, col:22> 'int' '+'
|   |     |-ImplicitCastExpr 0x7fdb73876cc8 <col:18> 'int' <LValueToRValue>
|   |     | `-DeclRefExpr 0x7fdb73876c80 <col:18> 'int' lvalue ParmVar 0x7fdb73876a58 'x' 'int'
|   |     `-IntegerLiteral 0x7fdb73876ca8 <col:22> 'int' 5
|   `-ReturnStmt 0x7fdb73876d60 <line:10:5, col:12>
|     `-ImplicitCastExpr 0x7fdb73876d48 <col:12> 'int' <LValueToRValue>
|       `-DeclRefExpr 0x7fdb73876d20 <col:12> 'int' lvalue ParmVar 0x7fdb73876a58 'x' 'int'
`-FunctionDecl 0x7fdb73876f20 <line:13:1, line:17:1> line:13:5 main 'int (int, char **)'
  |-ParmVarDecl 0x7fdb73876db0 <col:10, col:14> col:14 argc 'int'
  |-ParmVarDecl 0x7fdb73876e50 <col:20, col:27> col:27 argv 'char **'
  `-CompoundStmt 0x7fdb738770c8 <line:14:1, line:17:1>
    |-CallExpr 0x7fdb73877060 <line:15:5, col:14> 'int'
    | |-ImplicitCastExpr 0x7fdb73877048 <col:5> 'int (*)(int)' <FunctionToPointerDecay>
    | | `-DeclRefExpr 0x7fdb73876fd8 <col:5> 'int (int)' Function 0x7fdb73876b20 'addFive' 'int (int)'
    | `-IntegerLiteral 0x7fdb73877000 <col:13> 'int' 8
    `-ReturnStmt 0x7fdb738770b0 <line:16:5, col:12>
      `-IntegerLiteral 0x7fdb73877090 <col:12> 'int' 0
```

Output LLVM IR
```
$ clang -emit-llvm -S hello.c
```

```llvm
; ModuleID = 'hello.c'
source_filename = "hello.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.12.0"

; Function Attrs: nounwind ssp uwtable
define i32 @addFive(i32) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  %4 = load i32, i32* %2, align 4
  %5 = add nsw i32 %4, 5
  store i32 %5, i32* %3, align 4
  %6 = load i32, i32* %2, align 4
  ret i32 %6
}

; Function Attrs: nounwind ssp uwtable
define i32 @main(i32, i8**) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  store i8** %1, i8*** %5, align 8
  %6 = call i32 @addFive(i32 8)
  ret i32 0
}

attributes #0 = { nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"Apple LLVM version 8.1.0 (clang-802.0.42)"}
```
