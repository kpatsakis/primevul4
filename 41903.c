static int write_rom(struct edgeport_serial *serial, int start_address,
						int length, __u8 *buffer)
{
	if (serial->product_info.TiMode == TI_MODE_BOOT)
		return write_boot_mem(serial, start_address, length,
								buffer);

	if (serial->product_info.TiMode == TI_MODE_DOWNLOAD)
		return write_i2c_mem(serial, start_address, length,
						serial->TI_I2C_Type, buffer);
	return -EINVAL;
}
