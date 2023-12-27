#ifndef AFHLATON_XDG_SHELL_H
#define AFHLATON_XDG_SHELL_H

#include "wayland_headers.h"

struct afhlaton_toplevel {
	struct wl_list link;
	struct afhlaton_server *server;
	struct wlr_xdg_toplevel *xdg_toplevel;
	struct wlr_scene_tree *scene_tree;
	struct wl_listener map;
	struct wl_listener unmap;
	struct wl_listener destroy;
	struct wl_listener request_move;
	struct wl_listener request_resize;
	struct wl_listener request_maximize;
	struct wl_listener request_fullscreen;
};

void __listener_xdg_new_toplevel(struct wl_listener *listener, void *data);
void __listener_xdg_new_popup(struct wl_listener *listener, void *data);

void __listener_xdg_toplevel_map(struct wl_listener *listener, void *data);
void __listener_xdg_toplevel_unmap(struct wl_listener *listener, void *data);
void __listener_xdg_toplevel_destroy(struct wl_listener *listener, void *data);

void __listener_xdg_toplevel_request_move(struct wl_listener *listener, void *data);
void __listener_xdg_toplevel_request_resize(struct wl_listener *listener, void *data);
void __listener_xdg_toplevel_request_maximize(struct wl_listener *listener, void *data);
void __listener_xdg_toplevel_request_fullscreen(struct wl_listener *listener, void *data);

#endif
