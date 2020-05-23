#include <Button2.h>
#include <hal.hpp>

typedef void (*voidFuncPtr)();

void btnInit();
void btn1Callback(voidFuncPtr cb);
void btn2Callback(voidFuncPtr cb);

