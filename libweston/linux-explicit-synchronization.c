/*
 * Copyright © 2014, 2015 Collabora, Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "config.h"

#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>

#include "compositor.h"
#include "linux-explicit-synchronization.h"
#include "linux-explicit-synchronization-unstable-v1-server-protocol.h"

static void
linux_explicit_synchronization_sync_destroy(struct linux_explicit_synchronization_sync *sync)
{
	free(sync);
}

static void
synchronization_destroy(struct wl_client *client, struct wl_resource *resource)
{
	wl_resource_destroy(resource);
}

static void
synchronization_set_acquire_fence(struct wl_client *client,
				  struct wl_resource *resource,
				  int32_t fd)
{
	struct linux_explicit_synchronization_sync *sync;
	struct weston_surface *wsurface;

	sync = wl_resource_get_user_data(resource);

	assert(sync->sync_resource == resource);
	assert(sync->surface_resource);

	wsurface = wl_resource_get_user_data(sync->surface_resource);
	if (!wsurface)
		return;
	/*
	 * TODO: Check for fd to be a valid fence fd somehow.
	 */
	wsurface->pending.acquire_fence = fd;
	weston_log("got fence: %d\n", fd);
}

static const struct zcr_synchronization_v1_interface
zcr_synchronization_implementation = {
	synchronization_destroy,
	synchronization_set_acquire_fence,
};

static void
destroy_sync(struct wl_resource *sync_resource)
{
	struct linux_explicit_synchronization_sync *sync;

	sync = wl_resource_get_user_data(sync_resource);

	if (!sync)
		return;

	linux_explicit_synchronization_sync_destroy(sync);
}

static void
linux_explicit_synchronization_destroy(struct wl_client *client, struct wl_resource *resource)
{
	wl_resource_destroy(resource);
}

static void
linux_explicit_synchronization_get_synchroniztion(struct wl_client *client,
				    struct wl_resource *resource,
				    uint32_t id,
				    struct wl_resource *surface)
{
	struct weston_compositor *compositor;
	struct linux_explicit_synchronization_sync *sync;
	uint32_t version;

	version = wl_resource_get_version(resource);
	compositor = wl_resource_get_user_data(resource);

	/*if (??) {
		wl_resource_post_error(resource,
			ZCR_LINUX_EXPLICIT_SYNCHRONIZATION_V1_ERROR_SYNCHRONIZATION_EXISTS,
			"a synchronization already exists for this wl_surface");
		return;
	}*/

	sync = zalloc(sizeof *sync);
	if (!sync)
		goto err_out;

	sync->compositor = compositor;
	sync->surface_resource = surface;
	sync->sync_resource =
		wl_resource_create(client,
				   &zcr_synchronization_v1_interface,
				   version, id);
	if (!sync->sync_resource)
		goto err_dealloc;

	wl_resource_set_implementation(sync->sync_resource,
				       &zcr_synchronization_implementation,
				       sync, destroy_sync);

	return;

err_dealloc:
	free(sync);

err_out:
	wl_resource_post_no_memory(resource);
}

static const struct zcr_linux_explicit_synchronization_v1_interface linux_explicit_synchronization_implementation = {
	linux_explicit_synchronization_destroy,
	linux_explicit_synchronization_get_synchroniztion
};

static void
bind_linux_explicit_synchronization(struct wl_client *client,
				    void *data, uint32_t version, uint32_t id)
{
	struct weston_compositor *compositor = data;
	struct wl_resource *resource;

	resource = wl_resource_create(client,
				&zcr_linux_explicit_synchronization_v1_interface,
				version, id);
	if (resource == NULL) {
		wl_client_post_no_memory(client);
		return;
	}

	wl_resource_set_implementation(resource,
				       &linux_explicit_synchronization_implementation,
				       compositor, NULL);
}

/** Advertise linux_explicit_synchronization support
 *
 * Calling this initializes the zcr_linux_explicit_synchronization protocol
 * support, so that the interface will be advertised to clients. Essentially it
 * creates a global. Do not call this function multiple times in the compositor's
 * lifetime. There is no way to deinit explicitly, globals will be reaped
 * when the wl_display gets destroyed.
 *
 * \param compositor The compositor to init for.
 * \return Zero on success, -1 on failure.
 */
WL_EXPORT int
linux_explicit_synchronization_setup(struct weston_compositor *compositor)
{
	if (!wl_global_create(compositor->wl_display,
			      &zcr_linux_explicit_synchronization_v1_interface, 1,
			      compositor, bind_linux_explicit_synchronization))
		return -1;

	return 0;
}
