static __u8 *kye_consumer_control_fixup(struct hid_device *hdev, __u8 *rdesc,
		unsigned int *rsize, int offset, const char *device_name) {
	/*
	 * the fixup that need to be done:
	 *   - change Usage Maximum in the Comsumer Control
	 *     (report ID 3) to a reasonable value
	 */
	if (*rsize >= offset + 31 &&
	    /* Usage Page (Consumer Devices) */
	    rdesc[offset] == 0x05 && rdesc[offset + 1] == 0x0c &&
	    /* Usage (Consumer Control) */
	    rdesc[offset + 2] == 0x09 && rdesc[offset + 3] == 0x01 &&
	    /*   Usage Maximum > 12287 */
	    rdesc[offset + 10] == 0x2a && rdesc[offset + 12] > 0x2f) {
		hid_info(hdev, "fixing up %s report descriptor\n", device_name);
		rdesc[offset + 12] = 0x2f;
	}
	return rdesc;
}
