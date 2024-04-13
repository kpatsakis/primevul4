static int read_rom(struct edgeport_serial *serial,
				int start_address, int length, __u8 *buffer)
{
	int status;

	if (serial->product_info.TiMode == TI_MODE_DOWNLOAD) {
		status = read_download_mem(serial->serial->dev,
					       start_address,
					       length,
					       serial->TI_I2C_Type,
					       buffer);
	} else {
		status = read_boot_mem(serial, start_address, length,
								buffer);
	}
	return status;
}
