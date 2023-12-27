#ifndef AFHLATON_SEAT_H
#define AFHLATON_SEAT_H

#include "server.h"

void seat_init(struct afhlaton_server *server);

AFHLATON_LISTENER_SIG(seat_request_cursor);
AFHLATON_LISTENER_SIG(seat_request_set_selection);

#endif
