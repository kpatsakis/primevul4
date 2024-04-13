static int read_boot_mem(struct edgeport_serial *serial,
				int start_address, int length, __u8 *buffer)
{
	int status = 0;
	int i;

	for (i = 0; i < length; i++) {
		status = ti_vread_sync(serial->serial->dev,
				UMPC_MEMORY_READ, serial->TI_I2C_Type,
				(__u16)(start_address+i), &buffer[i], 0x01);
		if (status) {
			dev_dbg(&serial->serial->dev->dev, "%s - ERROR %x\n", __func__, status);
			return status;
		}
	}

	dev_dbg(&serial->serial->dev->dev, "%s - start_address = %x, length = %d\n",
		__func__, start_address, length);
	usb_serial_debug_data(&serial->serial->dev->dev, __func__, length, buffer);

	serial->TiReadI2C = 1;

	return status;
}
