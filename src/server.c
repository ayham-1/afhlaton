#include "server.h"

#include <assert.h>
#include <malloc.h>
#include <stdlib.h>

#include "output.h"
#include "xdg-shell.h"
#include "cursor.h"
#include "seat.h"

#include <../gen-protocols/xdg-shell-protocol.h>

int afhlaton_server_init(struct afhlaton_server *server) {
	wlr_log_init(WLR_DEBUG, NULL);

	server->wl_display = wl_display_create();

	server->backend = wlr_backend_autocreate(server->wl_display, NULL);
	assert(server->backend);

	if (NULL == server->backend) {
		wlr_log(WLR_ERROR, "failed to create wlr_backend");
		return 1;
	}

	server->renderer = wlr_renderer_autocreate(server->backend);
	if (NULL == server->renderer) {
		wlr_log(WLR_ERROR, "failed to create wlr_renderer");
		return 1;
	}
	wlr_renderer_init_wl_display(server->renderer, server->wl_display);

	server->allocator = wlr_allocator_autocreate(server->backend, server->renderer);
	if (NULL == server->allocator) {
		wlr_log(WLR_ERROR, "failed to create wlr_allocator");
	}

	wlr_compositor_create(server->wl_display, 5, server->renderer);
	wlr_subcompositor_create(server->wl_display);
	wlr_data_device_manager_create(server->wl_display);

	// outputs
	server->output_layout = wlr_output_layout_create(server->wl_display);
	wl_list_init(&server->outputs);
	server->new_output.notify = __listener_server_new_output;
	wl_signal_add(&server->backend->events.new_output, &server->new_output);

	// scenes
	server->scene = wlr_scene_create();
	server->scene_layout = wlr_scene_attach_output_layout(server->scene, server->output_layout);

	// xdg-shell
	wl_list_init(&server->toplevels);
	server->xdg_shell = wlr_xdg_shell_create(server->wl_display, 6);
	server->xdg_new_toplevel.notify = __listener_xdg_new_toplevel;
	wl_signal_add(&server->xdg_shell->events.new_toplevel, &server->xdg_new_toplevel);
	server->xdg_new_popup.notify = __listener_xdg_new_popup;
	wl_signal_add(&server->xdg_shell->events.new_popup, &server->xdg_new_popup);

	// cursor
	cursor_init(server);

	// inputs
	wl_list_init(&server->keyboards);
	AFHLATON_LISTENER_REG(server->backend->events.new_input,
		       server->new_input, __listener_server_new_input);

	// seat
	seat_init(server);
	
	// server start
	const char *socket = wl_display_add_socket_auto(server->wl_display);
	if (!socket) {
		wlr_backend_destroy(server->backend);
		return 1;
	}

	if (!wlr_backend_start(server->backend)) {
		wlr_backend_destroy(server->backend);
		wl_display_destroy(server->wl_display);
		return 1;
	}

	setenv("WAYLAND_DISPLAY", socket, true);

	wlr_log(WLR_INFO, "Running afhlaton on WAYLAND_DISPLAY=%s", socket);
	wl_display_run(server->wl_display);

	return 0;
}

void afhlaton_server_cleanup(struct afhlaton_server *server) {
	wl_display_destroy_clients(server->wl_display);
	wlr_scene_node_destroy(&server->scene->tree.node);
	cursor_clean(server);
	wl_display_destroy(server->wl_display);
}

void afhlaton_server_new_pointer(struct afhlaton_server* server, struct wlr_input_device *device) {
	wlr_log(WLR_INFO, "new pointer");
	wlr_cursor_attach_input_device(server->cursor, device);
}

struct afhlaton_toplevel *afhlaton_server_get_toplevel_at(struct afhlaton_server *server,
							  double lx, double ly,
							  struct wlr_surface **surface,
							  double *sx, double *sy) {
	struct wlr_scene_node *node = wlr_scene_node_at(&server->scene->tree.node, lx, ly, sx, sy);
	if (NULL == node || WLR_SCENE_NODE_BUFFER != node->type) {
		return NULL;
	}

	struct wlr_scene_buffer *scene_buffer = wlr_scene_buffer_from_node(node);
	struct wlr_scene_surface *scene_surface = wlr_scene_surface_try_from_buffer(scene_buffer);
	if (!scene_buffer) {
		return NULL;
	}

	*surface = scene_surface->surface;
	struct wlr_scene_tree *tree = node->parent;
	while (NULL != tree && NULL == tree->node.data) {
		tree = tree->node.parent;
	}
	return tree->node.data;
}

AFHLATON_LISTENER_SIG(server_new_output) {
	struct afhlaton_server* server = wl_container_of(listener, server, new_output);
	struct wlr_output *wlr_output = data;

	wlr_output_init_render(wlr_output, server->allocator, server->renderer);

	struct wlr_output_state state;
	wlr_output_state_init(&state);
	wlr_output_state_set_enabled(&state, true);

	struct wlr_output_mode* mode = wlr_output_preferred_mode(wlr_output);
	if (NULL != mode) {
		wlr_output_state_set_mode(&state, mode);
	}

	wlr_output_commit_state(wlr_output, &state);
	wlr_output_state_finish(&state);

	struct afhlaton_output *output = calloc(1, sizeof(*output));
	output->wlr_output = wlr_output;
	output->server = server;

	output->frame.notify = __listener_output_frame;
	wl_signal_add(&wlr_output->events.frame, &output->frame);

	output->request_state.notify = __listener_output_request_state;
	wl_signal_add(&wlr_output->events.request_state, &output->request_state);

	output->destroy.notify = __listener_output_destroy;
	wl_signal_add(&wlr_output->events.destroy, &output->destroy);

	wl_list_insert(&server->outputs, &output->link);

	struct wlr_output_layout_output *l_output = wlr_output_layout_add_auto(server->output_layout, wlr_output);
	struct wlr_scene_output *scene_output = wlr_scene_output_create(server->scene, wlr_output);
	wlr_scene_output_layout_add_output(server->scene_layout, l_output, scene_output);
}

AFHLATON_LISTENER_SIG(server_new_input) {
	struct afhlaton_server *server = wl_container_of(listener, server, new_input);
	struct wlr_input_device *device = data;

	switch(device->type) {
		case WLR_INPUT_DEVICE_KEYBOARD:
			break;
		case WLR_INPUT_DEVICE_POINTER:
			afhlaton_server_new_pointer(server, device);
			break;
		default:
			break;
	};

	uint32_t caps = WL_SEAT_CAPABILITY_POINTER;
	if (!wl_list_empty(&server->keyboards)) {
		caps |= WL_SEAT_CAPABILITY_KEYBOARD;
	}
	wlr_seat_set_capabilities(server->seat, caps);
}
