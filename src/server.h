#ifndef AFHLATON_SERVER
#define AFHLATON_SERVER

#include "macros.h"
#include "wayland_headers.h"

struct afhlaton_toplevel;

struct afhlaton_server {
	// main wayland stuff
	struct wl_display *wl_display;
	struct wlr_backend *backend;
	struct wlr_renderer *renderer;

	// physical outputs
	struct wl_list outputs;
	struct wlr_output_layout *output_layout;
	struct wl_listener new_output;

	// render outputs
	struct wlr_scene *scene;
	struct wlr_scene_output_layout *scene_layout;

	// xdg-shell
	struct wlr_xdg_shell *xdg_shell;
	struct wl_listener xdg_new_toplevel;
	struct wl_listener xdg_new_popup;
	struct wl_list toplevels;

	// cursor
	struct wlr_cursor *cursor;
	struct wlr_xcursor_manager *cursor_mgr;
	struct wl_listener cursor_motion;
	struct wl_listener cursor_motion_absolute;
	struct wl_listener cursor_button;
	struct wl_listener cursor_axis;
	struct wl_listener cursor_frame;
	enum afhlaton_cursor_mode {
		CUR_PASSTHROUGH,
		CUR_MOVE,
		CUR_RESIZE,
	} cursor_mode;
	struct afhlaton_toplevel *grabbed_toplevel;
	double grab_x, grab_y;
	struct wlr_box grab_geobox;
	uint32_t resize_edges;

	// input
	struct wl_list keyboards;
	struct wl_listener new_input;

	// seat
	struct wlr_seat *seat;
	struct wl_listener seat_request_cursor;
	struct wl_listener seat_request_set_selection;
	
	// auxiliary
	struct wlr_allocator *allocator;
	struct wl_event_loop *wl_event_loop;
};

int afhlaton_server_init(struct afhlaton_server *server);
void afhlaton_server_cleanup(struct afhlaton_server *server);

void afhlaton_server_new_pointer(struct afhlaton_server* server, struct wlr_input_device *device);

struct afhlaton_toplevel *afhlaton_server_get_toplevel_at(struct afhlaton_server *server,
							  double lx, double ly,
							  struct wlr_surface **surface,
							  double *sx, double *sy);

AFHLATON_LISTENER_SIG(server_new_output);
AFHLATON_LISTENER_SIG(server_new_input);

#endif
