#include <printf.h>
#include <assert.h>
#include <stdlib.h>

#include "wayland_headers.h"
#include "server.h"

int main(int argc, char* argv[]) {
	(void) argc;
	(void) argv;

	struct afhlaton_server server = {0};

	afhlaton_server_init(&server);
	afhlaton_server_cleanup(&server);

	return 0;
}
