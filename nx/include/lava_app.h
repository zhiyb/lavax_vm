#pragma once

#include "lava.h"

typedef enum {
    LavaAppStopped = 0,
    LavaAppNop,
    LavaAppRefresh,
    LavaAppCheckKey,
} lava_app_op_t;

int lava_init();
lava_app_op_t lava_loop();
Lava &lava_inst();
void lava_update_pad(u64 key);
