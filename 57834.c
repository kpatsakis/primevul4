static void edge_heartbeat_work(struct work_struct *work)
{
	struct edgeport_serial *serial;
	struct ti_i2c_desc *rom_desc;

	serial = container_of(work, struct edgeport_serial,
			heartbeat_work.work);

	rom_desc = kmalloc(sizeof(*rom_desc), GFP_KERNEL);

	/* Descriptor address request is enough to reset the firmware timer */
	if (!rom_desc || !get_descriptor_addr(serial, I2C_DESC_TYPE_ION,
			rom_desc)) {
		dev_err(&serial->serial->interface->dev,
				"%s - Incomplete heartbeat\n", __func__);
	}
	kfree(rom_desc);

	edge_heartbeat_schedule(serial);
}
