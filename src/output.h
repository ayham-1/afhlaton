#ifndef AFHLATON_OUTPUT_H
#define AFHLATON_OUTPUT_H

#include "macros.h"
#include "wayland_headers.h"

struct afhlaton_server;

struct afhlaton_output {
	struct wl_list link;
	struct afhlaton_server *server;
	struct wlr_output  *wlr_output;
	struct wl_listener frame;
	struct wl_listener request_state;
	struct wl_listener destroy;
};

AFHLATON_LISTENER_SIG(output_frame);
AFHLATON_LISTENER_SIG(output_request_state);
AFHLATON_LISTENER_SIG(output_destroy);

#endif
