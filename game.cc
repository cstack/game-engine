#include <ppapi_simple/ps_main.h>
#include <ppapi_simple/ps_context_2d.h>
#include <iostream>

void update(double dt, PSContext2D_t* ctx) {
  // Update and render
  PSContext2DGetBuffer(ctx);

  if (NULL == ctx->data) return;

  // Clear to WHITE
  memset(ctx->data, 0xFF, ctx->stride * ctx->height);

  PSContext2DSwapBuffer(ctx);
}

int game_main(int argc, char* argv[]) {

  PSEventSetFilter(PSE_ALL);

  PSContext2D_t* ctx = PSContext2DAllocate(PP_IMAGEDATAFORMAT_BGRA_PREMUL);
  while (1) {
    PSEvent* event;

    // Consume all available events
    while ((event = PSEventTryAcquire()) != NULL) {
      PSContext2DHandleEvent(ctx, event);
      PSEventRelease(event);
    }

    if (ctx->bound) {
      update(1.0/30, ctx);
    } else {
      // If not bound, wait for an event which may signal a context becoming
      // available, instead of spinning.
      event = PSEventWaitAcquire();
      PSContext2DHandleEvent(ctx, event);
      PSEventRelease(event);
    }
  }

  return 0;
}

PPAPI_SIMPLE_REGISTER_MAIN(game_main)