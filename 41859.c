static int check_i2c_image(struct edgeport_serial *serial)
{
	struct device *dev = &serial->serial->dev->dev;
	int status = 0;
	struct ti_i2c_desc *rom_desc;
	int start_address = 2;
	__u8 *buffer;
	__u16 ttype;

	rom_desc = kmalloc(sizeof(*rom_desc), GFP_KERNEL);
	if (!rom_desc) {
		dev_err(dev, "%s - out of memory\n", __func__);
		return -ENOMEM;
	}
	buffer = kmalloc(TI_MAX_I2C_SIZE, GFP_KERNEL);
	if (!buffer) {
		dev_err(dev, "%s - out of memory when allocating buffer\n",
								__func__);
		kfree(rom_desc);
		return -ENOMEM;
	}

	/* Read the first byte (Signature0) must be 0x52 or 0x10 */
	status = read_rom(serial, 0, 1, buffer);
	if (status)
		goto out;

	if (*buffer != UMP5152 && *buffer != UMP3410) {
		dev_err(dev, "%s - invalid buffer signature\n", __func__);
		status = -ENODEV;
		goto out;
	}

	do {
		/* Validate the I2C */
		status = read_rom(serial,
				start_address,
				sizeof(struct ti_i2c_desc),
				(__u8 *)rom_desc);
		if (status)
			break;

		if ((start_address + sizeof(struct ti_i2c_desc) +
					rom_desc->Size) > TI_MAX_I2C_SIZE) {
			status = -ENODEV;
			dev_dbg(dev, "%s - structure too big, erroring out.\n", __func__);
			break;
		}

		dev_dbg(dev, "%s Type = 0x%x\n", __func__, rom_desc->Type);

		/* Skip type 2 record */
		ttype = rom_desc->Type & 0x0f;
		if (ttype != I2C_DESC_TYPE_FIRMWARE_BASIC
			&& ttype != I2C_DESC_TYPE_FIRMWARE_AUTO) {
			/* Read the descriptor data */
			status = read_rom(serial, start_address +
						sizeof(struct ti_i2c_desc),
						rom_desc->Size, buffer);
			if (status)
				break;

			status = valid_csum(rom_desc, buffer);
			if (status)
				break;
		}
		start_address = start_address + sizeof(struct ti_i2c_desc) +
								rom_desc->Size;

	} while ((rom_desc->Type != I2C_DESC_TYPE_ION) &&
				(start_address < TI_MAX_I2C_SIZE));

	if ((rom_desc->Type != I2C_DESC_TYPE_ION) ||
				(start_address > TI_MAX_I2C_SIZE))
		status = -ENODEV;

out:
	kfree(buffer);
	kfree(rom_desc);
	return status;
}
