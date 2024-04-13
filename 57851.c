static int get_descriptor_addr(struct edgeport_serial *serial,
				int desc_type, struct ti_i2c_desc *rom_desc)
{
	int start_address;
	int status;

	/* Search for requested descriptor in I2C */
	start_address = 2;
	do {
		status = read_rom(serial,
				   start_address,
				   sizeof(struct ti_i2c_desc),
				   (__u8 *)rom_desc);
		if (status)
			return 0;

		if (rom_desc->Type == desc_type)
			return start_address;

		start_address = start_address + sizeof(struct ti_i2c_desc) +
						le16_to_cpu(rom_desc->Size);

	} while ((start_address < TI_MAX_I2C_SIZE) && rom_desc->Type);

	return 0;
}
