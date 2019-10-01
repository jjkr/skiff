#include <binaryen-c.h>

int main()
{
    BinaryenModuleRef module = BinaryenModuleCreate();
    BinaryenModulePrint(module);
    BinaryenModuleDispose(module);
}
