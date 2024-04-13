static int logi_dj_ll_parse(struct hid_device *hid)
{
	struct dj_device *djdev = hid->driver_data;
	unsigned int rsize = 0;
	char *rdesc;
	int retval;

	dbg_hid("%s\n", __func__);

	djdev->hdev->version = 0x0111;
	djdev->hdev->country = 0x00;

	rdesc = kmalloc(MAX_RDESC_SIZE, GFP_KERNEL);
	if (!rdesc)
		return -ENOMEM;

	if (djdev->reports_supported & STD_KEYBOARD) {
		dbg_hid("%s: sending a kbd descriptor, reports_supported: %x\n",
			__func__, djdev->reports_supported);
		rdcat(rdesc, &rsize, kbd_descriptor, sizeof(kbd_descriptor));
	}

	if (djdev->reports_supported & STD_MOUSE) {
		dbg_hid("%s: sending a mouse descriptor, reports_supported: "
			"%x\n", __func__, djdev->reports_supported);
		rdcat(rdesc, &rsize, mse_descriptor, sizeof(mse_descriptor));
	}

	if (djdev->reports_supported & MULTIMEDIA) {
		dbg_hid("%s: sending a multimedia report descriptor: %x\n",
			__func__, djdev->reports_supported);
		rdcat(rdesc, &rsize, consumer_descriptor, sizeof(consumer_descriptor));
	}

	if (djdev->reports_supported & POWER_KEYS) {
		dbg_hid("%s: sending a power keys report descriptor: %x\n",
			__func__, djdev->reports_supported);
		rdcat(rdesc, &rsize, syscontrol_descriptor, sizeof(syscontrol_descriptor));
	}

	if (djdev->reports_supported & MEDIA_CENTER) {
		dbg_hid("%s: sending a media center report descriptor: %x\n",
			__func__, djdev->reports_supported);
		rdcat(rdesc, &rsize, media_descriptor, sizeof(media_descriptor));
	}

	if (djdev->reports_supported & KBD_LEDS) {
		dbg_hid("%s: need to send kbd leds report descriptor: %x\n",
			__func__, djdev->reports_supported);
	}

	retval = hid_parse_report(hid, rdesc, rsize);
	kfree(rdesc);

	return retval;
}
