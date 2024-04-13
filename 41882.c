static int get_manuf_info(struct edgeport_serial *serial, __u8 *buffer)
{
	int status;
	int start_address;
	struct ti_i2c_desc *rom_desc;
	struct edge_ti_manuf_descriptor *desc;
	struct device *dev = &serial->serial->dev->dev;

	rom_desc = kmalloc(sizeof(*rom_desc), GFP_KERNEL);
	if (!rom_desc) {
		dev_err(dev, "%s - out of memory\n", __func__);
		return -ENOMEM;
	}
	start_address = get_descriptor_addr(serial, I2C_DESC_TYPE_ION,
								rom_desc);

	if (!start_address) {
		dev_dbg(dev, "%s - Edge Descriptor not found in I2C\n", __func__);
		status = -ENODEV;
		goto exit;
	}

	/* Read the descriptor data */
	status = read_rom(serial, start_address+sizeof(struct ti_i2c_desc),
						rom_desc->Size, buffer);
	if (status)
		goto exit;

	status = valid_csum(rom_desc, buffer);

	desc = (struct edge_ti_manuf_descriptor *)buffer;
	dev_dbg(dev, "%s - IonConfig      0x%x\n", __func__, desc->IonConfig);
	dev_dbg(dev, "%s - Version          %d\n", __func__, desc->Version);
	dev_dbg(dev, "%s - Cpu/Board      0x%x\n", __func__, desc->CpuRev_BoardRev);
	dev_dbg(dev, "%s - NumPorts         %d\n", __func__, desc->NumPorts);
	dev_dbg(dev, "%s - NumVirtualPorts  %d\n", __func__, desc->NumVirtualPorts);
	dev_dbg(dev, "%s - TotalPorts       %d\n", __func__, desc->TotalPorts);

exit:
	kfree(rom_desc);
	return status;
}
