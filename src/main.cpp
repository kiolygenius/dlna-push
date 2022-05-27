#include "PushService.h"
#include <iostream>

static char *action = NULL;
static char *url = NULL;
static char *udid = NULL;

static GOptionEntry entries[] =
{
    {"action", 'a', 0, G_OPTION_ARG_STRING, &action, "Do action, only 'list', 'push' available.", "ACTION"},
    {"url", 'u', 0, G_OPTION_ARG_STRING, &url, "Play with url media when action is push.", "URL"},
    {"udid", 'd', 0, G_OPTION_ARG_STRING, &udid, "Device to push playing.", "UDID"},
    { NULL }
};

static gboolean idle_add_func(gpointer data);
struct context
{
    std::unique_ptr<PushService> pService;
    GMainLoop *pLoop;
};
static std::shared_ptr<context> pContext;

int main(int argc, char **argv)
{
    GError *error = NULL;
    auto r_ctx = g_option_context_new ("- UPnP AV control point");
    g_option_context_add_main_entries (r_ctx, entries, "dlna-cast");
    if (!g_option_context_parse(r_ctx, &argc, &argv, &error)) 
    {
		g_print("option parsing failed: %s\n", error->message);
		return 1;
	}
    do {
        pContext = std::shared_ptr<context>(new context {std::make_unique<PushService>(), NULL});
        pContext->pService->InitUPnp(0);
        if (action)
        {
            if (strncmp("list", action, 4) == 0)
            {
                pContext->pService->List(5);
            }
            else if (strncmp("push", action, 4) == 0)
            {
                pContext->pService->Play(udid, url);
            }
            else
            {
                std::cerr << "unknown action.\n";
            }
        }
        else
        {
            std::cerr << "no action.\n";
            break;
        }
        auto main_loop = g_main_loop_new(NULL, FALSE);
        pContext->pLoop = main_loop;
        g_idle_add(idle_add_func, &pContext);
        g_main_loop_run(main_loop);
        g_main_loop_unref(main_loop);
        g_option_context_free(r_ctx);
    } while (0);
    pContext->pLoop = nullptr;
    pContext->pService = nullptr;
    pContext = nullptr;
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

