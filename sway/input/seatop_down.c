#define _POSIX_C_SOURCE 200809L
#include <wlr/types/wlr_cursor.h>
#include "sway/input/cursor.h"
#include "sway/input/seat.h"
#include "sway/tree/view.h"
#include "log.h"

struct seatop_down_event {
	struct sway_container *con;
	double ref_lx, ref_ly;         // cursor's x/y at start of op
	double ref_con_lx, ref_con_ly; // container's x/y at start of op
};

static void handle_button(struct sway_seat *seat, uint32_t time_msec,
		struct wlr_input_device *device, uint32_t button,
		enum wlr_button_state state) {
	seat_pointer_notify_button(seat, time_msec, button, state);

	if (seat->cursor->pressed_button_count == 0) {
		seatop_begin_default(seat);
	}
}

static void handle_motion(struct sway_seat *seat, uint32_t time_msec,
		double dx, double dy) {
	struct seatop_down_event *e = seat->seatop_data;
	struct sway_container *con = e->con;
	if (seat_is_input_allowed(seat, con->view->surface)) {
		double moved_x = seat->cursor->cursor->x - e->ref_lx;
		double moved_y = seat->cursor->cursor->y - e->ref_ly;
		double sx = e->ref_con_lx + moved_x;
		double sy = e->ref_con_ly + moved_y;
		wlr_seat_pointer_notify_motion(seat->wlr_seat, time_msec, sx, sy);
	}
}

static void handle_unref(struct sway_seat *seat, struct sway_container *con) {
	struct seatop_down_event *e = seat->seatop_data;
	if (e->con == con) {
		seatop_begin_default(seat);
	}
}

static const struct sway_seatop_impl seatop_impl = {
	.button = handle_button,
	.motion = handle_motion,
	.unref = handle_unref,
	.allow_set_cursor = true,
};

void seatop_begin_down(struct sway_seat *seat, struct sway_container *con,
		uint32_t time_msec, int sx, int sy) {
	seatop_end(seat);

	struct seatop_down_event *e =
		calloc(1, sizeof(struct seatop_down_event));
	if (!e) {
		return;
	}
	e->con = con;
	e->ref_lx = seat->cursor->cursor->x;
	e->ref_ly = seat->cursor->cursor->y;
	e->ref_con_lx = sx;
	e->ref_con_ly = sy;

	seat->seatop_impl = &seatop_impl;
	seat->seatop_data = e;

	container_raise_floating(con);
}
