static int i2c_type_bootmode(struct edgeport_serial *serial)
{
	struct device *dev = &serial->serial->dev->dev;
	int status;
	u8 *data;

	data = kmalloc(1, GFP_KERNEL);
	if (!data) {
		dev_err(dev, "%s - out of memory\n", __func__);
		return -ENOMEM;
	}

	/* Try to read type 2 */
	status = ti_vread_sync(serial->serial->dev, UMPC_MEMORY_READ,
				DTK_ADDR_SPACE_I2C_TYPE_II, 0, data, 0x01);
	if (status)
		dev_dbg(dev, "%s - read 2 status error = %d\n", __func__, status);
	else
		dev_dbg(dev, "%s - read 2 data = 0x%x\n", __func__, *data);
	if ((!status) && (*data == UMP5152 || *data == UMP3410)) {
		dev_dbg(dev, "%s - ROM_TYPE_II\n", __func__);
		serial->TI_I2C_Type = DTK_ADDR_SPACE_I2C_TYPE_II;
		goto out;
	}

	/* Try to read type 3 */
	status = ti_vread_sync(serial->serial->dev, UMPC_MEMORY_READ,
				DTK_ADDR_SPACE_I2C_TYPE_III, 0,	data, 0x01);
	if (status)
		dev_dbg(dev, "%s - read 3 status error = %d\n", __func__, status);
	else
		dev_dbg(dev, "%s - read 2 data = 0x%x\n", __func__, *data);
	if ((!status) && (*data == UMP5152 || *data == UMP3410)) {
		dev_dbg(dev, "%s - ROM_TYPE_III\n", __func__);
		serial->TI_I2C_Type = DTK_ADDR_SPACE_I2C_TYPE_III;
		goto out;
	}

	dev_dbg(dev, "%s - Unknown\n", __func__);
	serial->TI_I2C_Type = DTK_ADDR_SPACE_I2C_TYPE_II;
	status = -ENODEV;
out:
	kfree(data);
	return status;
}
