#ifndef __CLOSED_LOOP__H
#define __CLOSED_LOOP__H

#include "main.h"
#include "adcs.h"
#include "pwms.h"
#include "controllers.h"

int closedLoop(PIController* PI, float ovRef);

#endif