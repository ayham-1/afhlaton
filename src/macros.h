#ifndef AFHLATON_MACROS_H
#define AFHLATON_MACROS_H

#include "wayland_headers.h"

#define AFHLATON_LISTENER_REG(evt, wl_listener_var, listener_func) 	\
		wl_listener_var.notify = listener_func; 		\
		wl_signal_add(&evt, &wl_listener_var);

#define AFHLATON_LISTENER_SIG(evt_name) \
	void __listener_##evt_name(struct wl_listener *listener, void *data)

#endif
