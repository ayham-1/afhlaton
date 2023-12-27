#ifndef AFHLATON_CURSOR_H
#define AFHLATON_CURSOR_H

#include "macros.h"
#include "server.h"

void cursor_init(struct afhlaton_server *server);
void cursor_clean(struct afhlaton_server *server);

void cursor_reset_mode(struct afhlaton_server *server);

void cursor_process_motion(struct afhlaton_server *server, uint32_t time);

AFHLATON_LISTENER_SIG(cursor_motion);
AFHLATON_LISTENER_SIG(cursor_motion_absolute);
AFHLATON_LISTENER_SIG(cursor_button);
AFHLATON_LISTENER_SIG(cursor_axis);
AFHLATON_LISTENER_SIG(cursor_frame);

#endif
