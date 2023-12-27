#include "cursor.h"

#include "server.h"

void cursor_init(struct afhlaton_server *server) {
	server->cursor = wlr_cursor_create();
	wlr_cursor_attach_output_layout(server->cursor, server->output_layout);
	server->cursor_mgr = wlr_xcursor_manager_create(NULL, 24);
	//wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");

	server->cursor_mode = CUR_PASSTHROUGH;

	AFHLATON_LISTENER_REG(server->cursor->events.motion, server->cursor_motion, __listener_cursor_motion);
	AFHLATON_LISTENER_REG(server->cursor->events.motion_absolute, server->cursor_motion_absolute, __listener_cursor_motion_absolute);
	AFHLATON_LISTENER_REG(server->cursor->events.button, server->cursor_button, __listener_cursor_button);
	AFHLATON_LISTENER_REG(server->cursor->events.axis, server->cursor_axis, __listener_cursor_axis);
	AFHLATON_LISTENER_REG(server->cursor->events.frame, server->cursor_frame, __listener_cursor_frame);

}

void cursor_clean(struct afhlaton_server *server) {
	wlr_xcursor_manager_destroy(server->cursor_mgr);
}

void cursor_reset_mode(struct afhlaton_server *server) {
	server->cursor_mode = CUR_PASSTHROUGH;
	server->grabbed_toplevel = NULL;
}

void cursor_process_motion(struct afhlaton_server *server, uint32_t time) {
	if (CUR_MOVE == server->cursor_mode) {
		//cursor_process_move(server, time);
		return;
	} else if (CUR_RESIZE == server->cursor_mode) {
		//cursor_process_resize(server, time);
		return;
	}

	// TODO(ayham-1): delegate to appropriate toplevel
	double sx, sy;
	struct wlr_seat *seat = server->seat;
	struct wlr_surface *surface = NULL;
	struct afhlaton_toplevel *toplevel = afhlaton_server_get_toplevel_at(server,
		server->cursor->x, server->cursor->y, &surface, &sx, &sy);
	if (!toplevel) {
		wlr_cursor_set_xcursor(server->cursor, server->cursor_mgr, "default");
	}
	if (surface) {
		wlr_seat_pointer_notify_enter(seat, surface, sx, sy);
		wlr_seat_pointer_notify_motion(seat, time, sx, sy);
	} else {
		wlr_seat_pointer_clear_focus(seat);
	}
}

AFHLATON_LISTENER_SIG(cursor_motion) {
	struct afhlaton_server *server = wl_container_of(listener, server, cursor_motion);
	struct wlr_pointer_motion_event *event = data;

	wlr_cursor_move(server->cursor, &event->pointer->base, event->delta_x, event->delta_y);

	cursor_process_motion(server, event->time_msec);
}

AFHLATON_LISTENER_SIG(cursor_motion_absolute) {
	struct afhlaton_server *server = wl_container_of(listener, server, cursor_motion_absolute);
	struct wlr_pointer_motion_absolute_event *event = data;
	wlr_cursor_warp_absolute(server->cursor, &event->pointer->base, event->x, event->y);

	cursor_process_motion(server, event->time_msec);
}

AFHLATON_LISTENER_SIG(cursor_button) {
	struct afhlaton_server *server = wl_container_of(listener, server, cursor_button);
	struct wlr_pointer_button_event *event = data;

	wlr_seat_pointer_notify_button(server->seat, event->time_msec, event->button, event->state);

	double sx, sy;
	struct wlr_surface *surface = NULL;
	struct afhlaton_toplevel *toplevel = afhlaton_server_get_toplevel_at(server, server->cursor->x, server->cursor->y, &surface, &sx, &sy);

	if (WLR_BUTTON_RELEASED == event->state) {
		cursor_reset_mode(server);
	} else {
		//server_focus_toplevel(toplevel, surface);
	}
}

AFHLATON_LISTENER_SIG(cursor_axis) {
	struct afhlaton_server *server = wl_container_of(listener, server, cursor_axis);
	struct wlr_pointer_axis_event *event = data;

	wlr_seat_pointer_notify_axis(server->seat, event->time_msec, event->orientation,
			      event->delta, event->delta_discrete, event->source);
}

AFHLATON_LISTENER_SIG(cursor_frame) {
	(void)data;
	struct afhlaton_server *server = wl_container_of(listener, server, cursor_frame);
	wlr_seat_pointer_notify_frame(server->seat);
}
