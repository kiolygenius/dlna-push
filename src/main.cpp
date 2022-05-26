#include "PushService.h"

static GOptionEntry entries[] =
{
        { NULL }
};

static gboolean idle_add_func(gpointer data);
struct context
{
    std::unique_ptr<PushService> pService;
    GMainLoop *pLoop;
};
static std::shared_ptr<context> pContext;

int main(int argc, char *argv[])
{

    auto r_ctx = g_option_context_new ("- UPnP AV control point");
    g_option_context_add_main_entries (r_ctx, entries, "dlna-cast");

    pContext = std::shared_ptr<context>(new context {std::make_unique<PushService>(), NULL});
    pContext->pService->InitUPnp(0);
    pContext->pService->List(5);
    
    auto main_loop = g_main_loop_new(NULL, FALSE);
    pContext->pLoop = main_loop;
    g_idle_add(idle_add_func, &pContext);
    g_main_loop_run(main_loop);
    g_main_loop_unref(main_loop);
    g_option_context_free(r_ctx);
    return 0;
}

static gboolean idle_add_func(gpointer data)
{
    const auto &ctx = *reinterpret_cast<std::shared_ptr<context>*>(data);
    if (ctx->pService->IsFinished())
    {
        g_main_loop_quit(ctx->pLoop);
        return FALSE;
    }

    ctx->pService->PoolOver();
    return TRUE;
}