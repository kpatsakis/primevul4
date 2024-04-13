static int write_boot_mem(struct edgeport_serial *serial,
				int start_address, int length, __u8 *buffer)
{
	int status = 0;
	int i;
	u8 *temp;

	/* Must do a read before write */
	if (!serial->TiReadI2C) {
		temp = kmalloc(1, GFP_KERNEL);
		if (!temp)
			return -ENOMEM;

		status = read_boot_mem(serial, 0, 1, temp);
		kfree(temp);
		if (status)
			return status;
	}

	for (i = 0; i < length; ++i) {
		status = ti_vsend_sync(serial->serial->dev, UMPC_MEMORY_WRITE,
				buffer[i], (u16)(i + start_address), NULL,
				0, TI_VSEND_TIMEOUT_DEFAULT);
		if (status)
			return status;
	}

	dev_dbg(&serial->serial->dev->dev, "%s - start_sddr = %x, length = %d\n", __func__, start_address, length);
	usb_serial_debug_data(&serial->serial->dev->dev, __func__, length, buffer);

	return status;
}
