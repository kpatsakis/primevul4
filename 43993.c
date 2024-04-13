static void magicmouse_emit_touch(struct magicmouse_sc *msc, int raw_id, u8 *tdata)
{
	struct input_dev *input = msc->input;
	int id, x, y, size, orientation, touch_major, touch_minor, state, down;

	if (input->id.product == USB_DEVICE_ID_APPLE_MAGICMOUSE) {
		id = (tdata[6] << 2 | tdata[5] >> 6) & 0xf;
		x = (tdata[1] << 28 | tdata[0] << 20) >> 20;
		y = -((tdata[2] << 24 | tdata[1] << 16) >> 20);
		size = tdata[5] & 0x3f;
		orientation = (tdata[6] >> 2) - 32;
		touch_major = tdata[3];
		touch_minor = tdata[4];
		state = tdata[7] & TOUCH_STATE_MASK;
		down = state != TOUCH_STATE_NONE;
	} else { /* USB_DEVICE_ID_APPLE_MAGICTRACKPAD */
		id = (tdata[7] << 2 | tdata[6] >> 6) & 0xf;
		x = (tdata[1] << 27 | tdata[0] << 19) >> 19;
		y = -((tdata[3] << 30 | tdata[2] << 22 | tdata[1] << 14) >> 19);
		size = tdata[6] & 0x3f;
		orientation = (tdata[7] >> 2) - 32;
		touch_major = tdata[4];
		touch_minor = tdata[5];
		state = tdata[8] & TOUCH_STATE_MASK;
		down = state != TOUCH_STATE_NONE;
	}

	/* Store tracking ID and other fields. */
	msc->tracking_ids[raw_id] = id;
	msc->touches[id].x = x;
	msc->touches[id].y = y;
	msc->touches[id].size = size;

	/* If requested, emulate a scroll wheel by detecting small
	 * vertical touch motions.
	 */
	if (emulate_scroll_wheel) {
		unsigned long now = jiffies;
		int step_x = msc->touches[id].scroll_x - x;
		int step_y = msc->touches[id].scroll_y - y;

		/* Calculate and apply the scroll motion. */
		switch (state) {
		case TOUCH_STATE_START:
			msc->touches[id].scroll_x = x;
			msc->touches[id].scroll_y = y;

			/* Reset acceleration after half a second. */
			if (scroll_acceleration && time_before(now,
						msc->scroll_jiffies + HZ / 2))
				msc->scroll_accel = max_t(int,
						msc->scroll_accel - 1, 1);
			else
				msc->scroll_accel = SCROLL_ACCEL_DEFAULT;

			break;
		case TOUCH_STATE_DRAG:
			step_x /= (64 - (int)scroll_speed) * msc->scroll_accel;
			if (step_x != 0) {
				msc->touches[id].scroll_x -= step_x *
					(64 - scroll_speed) * msc->scroll_accel;
				msc->scroll_jiffies = now;
				input_report_rel(input, REL_HWHEEL, -step_x);
			}

			step_y /= (64 - (int)scroll_speed) * msc->scroll_accel;
			if (step_y != 0) {
				msc->touches[id].scroll_y -= step_y *
					(64 - scroll_speed) * msc->scroll_accel;
				msc->scroll_jiffies = now;
				input_report_rel(input, REL_WHEEL, step_y);
			}
			break;
		}
	}

	if (down)
		msc->ntouches++;

	input_mt_slot(input, id);
	input_mt_report_slot_state(input, MT_TOOL_FINGER, down);

	/* Generate the input events for this touch. */
	if (down) {
		input_report_abs(input, ABS_MT_TOUCH_MAJOR, touch_major << 2);
		input_report_abs(input, ABS_MT_TOUCH_MINOR, touch_minor << 2);
		input_report_abs(input, ABS_MT_ORIENTATION, -orientation);
		input_report_abs(input, ABS_MT_POSITION_X, x);
		input_report_abs(input, ABS_MT_POSITION_Y, y);

		if (report_undeciphered) {
			if (input->id.product == USB_DEVICE_ID_APPLE_MAGICMOUSE)
				input_event(input, EV_MSC, MSC_RAW, tdata[7]);
			else /* USB_DEVICE_ID_APPLE_MAGICTRACKPAD */
				input_event(input, EV_MSC, MSC_RAW, tdata[8]);
		}
	}
}
