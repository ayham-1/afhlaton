#include "xdg-shell.h"

#include <assert.h>
#include <malloc.h>

#include <../gen-protocols/xdg-shell-protocol.h>

#include "macros.h"
#include "server.h"

void __listener_xdg_new_toplevel(struct wl_listener *listener, void *data) {
	struct afhlaton_server* server = wl_container_of(listener, server, xdg_new_toplevel);
	struct wlr_xdg_toplevel *xdg_toplevel = data;

	struct afhlaton_toplevel *toplevel = calloc(1, sizeof(*toplevel));
	toplevel->server = server;
	toplevel->xdg_toplevel = xdg_toplevel;
	toplevel->scene_tree = wlr_scene_xdg_surface_create(&toplevel->server->scene->tree, xdg_toplevel->base);
	toplevel->scene_tree->node.data = toplevel;
	xdg_toplevel->base->data = toplevel->scene_tree;

	AFHLATON_LISTENER_REG(xdg_toplevel->base->surface->events.map, toplevel->map, __listener_xdg_toplevel_map);
	AFHLATON_LISTENER_REG(xdg_toplevel->base->surface->events.unmap, toplevel->unmap, __listener_xdg_toplevel_unmap);
	AFHLATON_LISTENER_REG(xdg_toplevel->base->surface->events.destroy, toplevel->destroy, __listener_xdg_toplevel_destroy);
	AFHLATON_LISTENER_REG(xdg_toplevel->events.request_move, toplevel->request_move, __listener_xdg_toplevel_request_move);
	AFHLATON_LISTENER_REG(xdg_toplevel->events.request_resize, toplevel->request_resize, __listener_xdg_toplevel_request_resize);
	AFHLATON_LISTENER_REG(xdg_toplevel->events.request_maximize, toplevel->request_maximize, __listener_xdg_toplevel_request_maximize);
	AFHLATON_LISTENER_REG(xdg_toplevel->events.request_fullscreen, toplevel->request_fullscreen, __listener_xdg_toplevel_request_fullscreen);
}

void __listener_xdg_new_popup(struct wl_listener *listener, void *data) {
	(void)listener;

	struct wlr_xdg_popup *xdg_popup = data;

	struct wlr_xdg_surface *parent = wlr_xdg_surface_try_from_wlr_surface(xdg_popup->parent);
	assert(NULL != parent);
	struct wlr_scene_tree *parent_tree = parent->data;
	xdg_popup->base->data = wlr_scene_xdg_surface_create(parent_tree, xdg_popup->base);
}

void __listener_xdg_toplevel_map(struct wl_listener *listener, void *data) {
	(void)data;

	struct afhlaton_toplevel *toplevel = wl_container_of(listener, toplevel, map);

	wl_list_insert(&toplevel->server->toplevels, &toplevel->link);
}

void __listener_xdg_toplevel_unmap(struct wl_listener *listener, void *data) {

}

void __listener_xdg_toplevel_destroy(struct wl_listener *listener, void *data) {}

void __listener_xdg_toplevel_request_move(struct wl_listener *listener, void *data) {}

void __listener_xdg_toplevel_request_resize(struct wl_listener *listener, void *data) {}

void __listener_xdg_toplevel_request_maximize(struct wl_listener *listener, void *data) {}

void __listener_xdg_toplevel_request_fullscreen(struct wl_listener *listener, void *data) {}
