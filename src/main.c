#include "../src/ratsel.h"

int main()
{
    int result = 0;
    RTSLApp app;

    d_clogf("%s", CLOG_INFO "App initialization...");

    result = RTSLApp__init(&app);
    if (result)
    {
        d_clogf("%s%i", CLOG_ERR "App failed initialization; Code: ", result);
        return RTSL_FAILURE;
    };

    d_clogf("%s", CLOG_INFO "App initialization successfully passed.");

    d_clogf("%s", CLOG_INFO "App running...");
    result = RTSLApp__run(&app);

    if (result)
    {
        d_clogf("%s%i", CLOG_ERR "App error during \"run\" function; Code: ", result);
        return RTSL_FAILURE;
    };

    d_clogf("%s", CLOG_INFO "App successfully exited.");
    return RTSL_SUCCESS;
};