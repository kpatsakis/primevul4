static int read_download_mem(struct usb_device *dev, int start_address,
				int length, __u8 address_type, __u8 *buffer)
{
	int status = 0;
	__u8 read_length;
	u16 be_start_address;

	dev_dbg(&dev->dev, "%s - @ %x for %d\n", __func__, start_address, length);

	/*
	 * Read in blocks of 64 bytes
	 * (TI firmware can't handle more than 64 byte reads)
	 */
	while (length) {
		if (length > 64)
			read_length = 64;
		else
			read_length = (__u8)length;

		if (read_length > 1) {
			dev_dbg(&dev->dev, "%s - @ %x for %d\n", __func__, start_address, read_length);
		}
		/*
		 * NOTE: Must use swab as wIndex is sent in little-endian
		 *       byte order regardless of host byte order.
		 */
		be_start_address = swab16((u16)start_address);
		status = ti_vread_sync(dev, UMPC_MEMORY_READ,
					(__u16)address_type,
					be_start_address,
					buffer, read_length);

		if (status) {
			dev_dbg(&dev->dev, "%s - ERROR %x\n", __func__, status);
			return status;
		}

		if (read_length > 1)
			usb_serial_debug_data(&dev->dev, __func__, read_length, buffer);

		/* Update pointers/length */
		start_address += read_length;
		buffer += read_length;
		length -= read_length;
	}

	return status;
}
