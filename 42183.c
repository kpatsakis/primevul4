static int hidp_setup_input(struct hidp_session *session,
				struct hidp_connadd_req *req)
{
	struct input_dev *input;
	int i;

	input = input_allocate_device();
	if (!input)
		return -ENOMEM;

	session->input = input;

	input_set_drvdata(input, session);

	input->name = "Bluetooth HID Boot Protocol Device";

	input->id.bustype = BUS_BLUETOOTH;
	input->id.vendor  = req->vendor;
	input->id.product = req->product;
	input->id.version = req->version;

	if (req->subclass & 0x40) {
		set_bit(EV_KEY, input->evbit);
		set_bit(EV_LED, input->evbit);
		set_bit(EV_REP, input->evbit);

		set_bit(LED_NUML,    input->ledbit);
		set_bit(LED_CAPSL,   input->ledbit);
		set_bit(LED_SCROLLL, input->ledbit);
		set_bit(LED_COMPOSE, input->ledbit);
		set_bit(LED_KANA,    input->ledbit);

		for (i = 0; i < sizeof(hidp_keycode); i++)
			set_bit(hidp_keycode[i], input->keybit);
		clear_bit(0, input->keybit);
	}

	if (req->subclass & 0x80) {
		input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
		input->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) |
			BIT_MASK(BTN_RIGHT) | BIT_MASK(BTN_MIDDLE);
		input->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y);
		input->keybit[BIT_WORD(BTN_MOUSE)] |= BIT_MASK(BTN_SIDE) |
			BIT_MASK(BTN_EXTRA);
		input->relbit[0] |= BIT_MASK(REL_WHEEL);
	}

	input->dev.parent = &session->conn->dev;

	input->event = hidp_input_event;

	return 0;
}
