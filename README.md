# Skiff
A modern language for the web

### Clang and LLVM

##### Links
 * [LLVM Tutorial](http://llvm.org/docs/tutorial/index.html)
 * [LLVM Language Manual](http://llvm.org/docs/LangRef.html)
 * [LLVM Programmer's Manual](http://llvm.org/docs/ProgrammersManual.html)
 * [LLVM doxygen](http://llvm.org/docs/doxygen/html/index.html)

##### Notes

Hello test file
```c
struct Pair
{
    int x;
    int y;
};

int addFive(int x)
{
    int result = x + 5;
    return result;
}

int main(int argc, char** argv)
{
    addFive(8);
    return 0;
}
```

Output clang AST
```
$ clang -Xclang -ast-dump -fsyntax-only foo.c
```

```
TranslationUnitDecl 0x7fde11023cd0 <<invalid sloc>> <invalid sloc>
|-TypedefDecl 0x7fde11024218 <<invalid sloc>> <invalid sloc> implicit __int128_t '__int128'
| `-BuiltinType 0x7fde11023f40 '__int128'
|-TypedefDecl 0x7fde11024278 <<invalid sloc>> <invalid sloc> implicit __uint128_t 'unsigned __int128'
| `-BuiltinType 0x7fde11023f60 'unsigned __int128'
|-TypedefDecl 0x7fde11024548 <<invalid sloc>> <invalid sloc> implicit __NSConstantString 'struct __NSConstantString_tag'
| `-RecordType 0x7fde11024350 'struct __NSConstantString_tag'
|   `-Record 0x7fde110242c8 '__NSConstantString_tag'
|-TypedefDecl 0x7fde110245d8 <<invalid sloc>> <invalid sloc> implicit __builtin_ms_va_list 'char *'
| `-PointerType 0x7fde110245a0 'char *'
|   `-BuiltinType 0x7fde11023d60 'char'
|-TypedefDecl 0x7fde11024898 <<invalid sloc>> <invalid sloc> implicit __builtin_va_list 'struct __va_list_tag [1]'
| `-ConstantArrayType 0x7fde11024840 'struct __va_list_tag [1]' 1
|   `-RecordType 0x7fde110246b0 'struct __va_list_tag'
|     `-Record 0x7fde11024628 '__va_list_tag'
|-RecordDecl 0x7fde110248e8 <hello.c:1:1, line:5:1> line:1:8 struct Pair definition
| |-FieldDecl 0x7fde110249a0 <line:3:5, col:9> col:9 x 'int'
| `-FieldDecl 0x7fde11018a00 <line:4:5, col:9> col:9 y 'int'
|-FunctionDecl 0x7fde11018b20 <line:7:1, line:11:1> line:7:5 used addFive 'int (int)'
| |-ParmVarDecl 0x7fde11018a58 <col:13, col:17> col:17 used x 'int'
| `-CompoundStmt 0x7fde11018d78 <line:8:1, line:11:1>
|   |-DeclStmt 0x7fde11018d08 <line:9:5, col:23>
|   | `-VarDecl 0x7fde11018c20 <col:5, col:22> col:9 used result 'int' cinit
|   |   `-BinaryOperator 0x7fde11018ce0 <col:18, col:22> 'int' '+'
|   |     |-ImplicitCastExpr 0x7fde11018cc8 <col:18> 'int' <LValueToRValue>
|   |     | `-DeclRefExpr 0x7fde11018c80 <col:18> 'int' lvalue ParmVar 0x7fde11018a58 'x' 'int'
|   |     `-IntegerLiteral 0x7fde11018ca8 <col:22> 'int' 5
|   `-ReturnStmt 0x7fde11018d60 <line:10:5, col:12>
|     `-ImplicitCastExpr 0x7fde11018d48 <col:12> 'int' <LValueToRValue>
|       `-DeclRefExpr 0x7fde11018d20 <col:12> 'int' lvalue Var 0x7fde11018c20 'result' 'int'
`-FunctionDecl 0x7fde11018f20 <line:13:1, line:17:1> line:13:5 main 'int (int, char **)'
  |-ParmVarDecl 0x7fde11018db0 <col:10, col:14> col:14 argc 'int'
  |-ParmVarDecl 0x7fde11018e50 <col:20, col:27> col:27 argv 'char **'
  `-CompoundStmt 0x7fde110190c8 <line:14:1, line:17:1>
    |-CallExpr 0x7fde11019060 <line:15:5, col:14> 'int'
    | |-ImplicitCastExpr 0x7fde11019048 <col:5> 'int (*)(int)' <FunctionToPointerDecay>
    | | `-DeclRefExpr 0x7fde11018fd8 <col:5> 'int (int)' Function 0x7fde11018b20 'addFive' 'int (int)'
    | `-IntegerLiteral 0x7fde11019000 <col:13> 'int' 8
    `-ReturnStmt 0x7fde110190b0 <line:16:5, col:12>
      `-IntegerLiteral 0x7fde11019090 <col:12> 'int' 0
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
  %6 = load i32, i32* %3, align 4
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
