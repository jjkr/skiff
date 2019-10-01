#include <binaryen-c.h>

int main()
{
    BinaryenModuleRef module = BinaryenModuleCreate();

    BinaryenType paramTypes[] = { BinaryenTypeInt32() };
    BinaryenFunctionTypeRef addFiveType = BinaryenAddFunctionType(
            module,
            "addFiveType",
            BinaryenTypeInt32(),
            paramTypes,
            1);

    BinaryenExpressionRef expr = BinaryenBinary(
            module,
            BinaryenAddInt32(),
            BinaryenLocalGet(module, 0, BinaryenTypeInt32()),
            BinaryenConst(module, BinaryenLiteralInt32(5)));
    BinaryenType varTypes[] = {};
    BinaryenAddFunction(
            module,
            "addFive",
            addFiveType,
            varTypes,
            0,
            expr);

    BinaryenModulePrint(module);
    BinaryenModuleDispose(module);
}
