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

#ifndef WESTON_LINUX_EXPLICIT_SYNCHRONIZATION_H
#define WESTON_LINUX_EXPLICIT_SYNCHRONIZATION_H

#include <stdint.h>

struct linux_explicit_synchronization_sync;
typedef void (*linux_explicit_synchronization_user_data_destroy_func)(
			struct linux_explicit_synchronization_sync *sync);

struct linux_explicit_synchronization_sync {
	struct wl_resource *sync_resource;
	struct wl_resource *surface_resource;
	struct weston_compositor *compositor;

	void *user_data;
	linux_explicit_synchronization_user_data_destroy_func user_data_destroy_func;
};

int
linux_explicit_synchronization_setup(struct weston_compositor *compositor);

#endif /* WESTON_LINUX_EXPLICIT_SYNCHRONIZATION_H */
