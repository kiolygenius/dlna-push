#include <chrono>
#include <thread>
#include "PushService.h"

int main(int argc, char *argv[])
{
    auto pService = std::make_unique<PushService>();
    pService->InitUPnp(0);
    pService->List(5);
    while (!pService->IsFinished())
    {
        pService->PoolOver();
        std::this_thread::sleep_for(std::chrono::duration<double, std::chrono::seconds::period>(0.1));
    }
    return 0;
}
