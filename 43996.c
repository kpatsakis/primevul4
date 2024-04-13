static int magicmouse_setup_input(struct input_dev *input, struct hid_device *hdev)
{
	int error;

	__set_bit(EV_KEY, input->evbit);

	if (input->id.product == USB_DEVICE_ID_APPLE_MAGICMOUSE) {
		__set_bit(BTN_LEFT, input->keybit);
		__set_bit(BTN_RIGHT, input->keybit);
		if (emulate_3button)
			__set_bit(BTN_MIDDLE, input->keybit);

		__set_bit(EV_REL, input->evbit);
		__set_bit(REL_X, input->relbit);
		__set_bit(REL_Y, input->relbit);
		if (emulate_scroll_wheel) {
			__set_bit(REL_WHEEL, input->relbit);
			__set_bit(REL_HWHEEL, input->relbit);
		}
	} else { /* USB_DEVICE_ID_APPLE_MAGICTRACKPAD */
		/* input->keybit is initialized with incorrect button info
		 * for Magic Trackpad. There really is only one physical
		 * button (BTN_LEFT == BTN_MOUSE). Make sure we don't
		 * advertise buttons that don't exist...
		 */
		__clear_bit(BTN_RIGHT, input->keybit);
		__clear_bit(BTN_MIDDLE, input->keybit);
		__set_bit(BTN_MOUSE, input->keybit);
		__set_bit(BTN_TOOL_FINGER, input->keybit);
		__set_bit(BTN_TOOL_DOUBLETAP, input->keybit);
		__set_bit(BTN_TOOL_TRIPLETAP, input->keybit);
		__set_bit(BTN_TOOL_QUADTAP, input->keybit);
		__set_bit(BTN_TOOL_QUINTTAP, input->keybit);
		__set_bit(BTN_TOUCH, input->keybit);
		__set_bit(INPUT_PROP_POINTER, input->propbit);
		__set_bit(INPUT_PROP_BUTTONPAD, input->propbit);
	}


	__set_bit(EV_ABS, input->evbit);

	error = input_mt_init_slots(input, 16, 0);
	if (error)
		return error;
	input_set_abs_params(input, ABS_MT_TOUCH_MAJOR, 0, 255 << 2,
			     4, 0);
	input_set_abs_params(input, ABS_MT_TOUCH_MINOR, 0, 255 << 2,
			     4, 0);
	input_set_abs_params(input, ABS_MT_ORIENTATION, -31, 32, 1, 0);

	/* Note: Touch Y position from the device is inverted relative
	 * to how pointer motion is reported (and relative to how USB
	 * HID recommends the coordinates work).  This driver keeps
	 * the origin at the same position, and just uses the additive
	 * inverse of the reported Y.
	 */
	if (input->id.product == USB_DEVICE_ID_APPLE_MAGICMOUSE) {
		input_set_abs_params(input, ABS_MT_POSITION_X,
				     MOUSE_MIN_X, MOUSE_MAX_X, 4, 0);
		input_set_abs_params(input, ABS_MT_POSITION_Y,
				     MOUSE_MIN_Y, MOUSE_MAX_Y, 4, 0);

		input_abs_set_res(input, ABS_MT_POSITION_X,
				  MOUSE_RES_X);
		input_abs_set_res(input, ABS_MT_POSITION_Y,
				  MOUSE_RES_Y);
	} else { /* USB_DEVICE_ID_APPLE_MAGICTRACKPAD */
		input_set_abs_params(input, ABS_X, TRACKPAD_MIN_X,
				     TRACKPAD_MAX_X, 4, 0);
		input_set_abs_params(input, ABS_Y, TRACKPAD_MIN_Y,
				     TRACKPAD_MAX_Y, 4, 0);
		input_set_abs_params(input, ABS_MT_POSITION_X,
				     TRACKPAD_MIN_X, TRACKPAD_MAX_X, 4, 0);
		input_set_abs_params(input, ABS_MT_POSITION_Y,
				     TRACKPAD_MIN_Y, TRACKPAD_MAX_Y, 4, 0);

		input_abs_set_res(input, ABS_X, TRACKPAD_RES_X);
		input_abs_set_res(input, ABS_Y, TRACKPAD_RES_Y);
		input_abs_set_res(input, ABS_MT_POSITION_X,
				  TRACKPAD_RES_X);
		input_abs_set_res(input, ABS_MT_POSITION_Y,
				  TRACKPAD_RES_Y);
	}

	input_set_events_per_packet(input, 60);

	if (report_undeciphered) {
		__set_bit(EV_MSC, input->evbit);
		__set_bit(MSC_RAW, input->mscbit);
	}

	return 0;
}
