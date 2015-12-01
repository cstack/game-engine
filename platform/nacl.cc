#include <ppapi_simple/ps_main.h>
#include <ppapi_simple/ps_context_2d.h>
#include <iostream>
#include <sys/time.h>

#include "platform.h"

double TARGET_FRAME_RATE = 60;

double seconds() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return tp.tv_sec + tp.tv_usec / 1000000.0;
}

int game_main(int argc, char* argv[]) {

  PSEventSetFilter(PSE_ALL);

  PSContext2D_t* ctx = PSContext2DAllocate(PP_IMAGEDATAFORMAT_BGRA_PREMUL);
  pixel_buffer_t* pixel_buffer = new pixel_buffer_t;

  double timeLastStep;
  double timeThisStep;
  double dt;
  while (1) {
    PSEvent* event;

    // Consume all available events
    while ((event = PSEventTryAcquire()) != NULL) {
      PSContext2DHandleEvent(ctx, event);
      PSEventRelease(event);
    }

    if (ctx->bound) {
      if(timeLastStep == 0)
        timeLastStep = seconds();
      timeThisStep = seconds();
      dt = timeThisStep - timeLastStep;
      PSContext2DGetBuffer(ctx);
      if (NULL == ctx->data) continue;
      pixel_buffer->height = ctx->height;
      pixel_buffer->stride = ctx->stride;
      pixel_buffer->data = ctx->data;
      update(dt, pixel_buffer);
      PSContext2DSwapBuffer(ctx);
      timeLastStep = timeThisStep;
      usleep(1000000/TARGET_FRAME_RATE);
    } else {
      // If not bound, wait for an event which may signal a context becoming
      // available, instead of spinning.
      event = PSEventWaitAcquire();
      PSContext2DHandleEvent(ctx, event);
      PSEventRelease(event);
    }
  }

  delete pixel_buffer;

  return 0;
}

PPAPI_SIMPLE_REGISTER_MAIN(game_main)