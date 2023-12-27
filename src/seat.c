#include "seat.h"

#include "macros.h"

void seat_init(struct afhlaton_server *server) {
	server->seat = wlr_seat_create(server->wl_display, "seat0");
	AFHLATON_LISTENER_REG(server->seat->events.request_set_cursor,
		       server->seat_request_cursor, __listener_seat_request_cursor);
	AFHLATON_LISTENER_REG(server->seat->events.request_set_selection,
		       server->seat_request_set_selection, 
		       __listener_seat_request_set_selection);
}

AFHLATON_LISTENER_SIG(seat_request_cursor) {}

AFHLATON_LISTENER_SIG(seat_request_set_selection) {}
