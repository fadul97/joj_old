#include "sandbox/gui_test.h"
#include "sandbox/app3d_test.h"
#include "joj/engine/engine.h"
#include "joj/core/jmacros.h"

int main()
{
    joj::Engine engine;
    if JOJ_FAILED(engine.start())
        return -1;

    App3DTest app;
    return engine.run(&app);
}
