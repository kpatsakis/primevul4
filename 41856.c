static int build_i2c_fw_hdr(__u8 *header, struct device *dev)
{
	__u8 *buffer;
	int buffer_size;
	int i;
	int err;
	__u8 cs = 0;
	struct ti_i2c_desc *i2c_header;
	struct ti_i2c_image_header *img_header;
	struct ti_i2c_firmware_rec *firmware_rec;
	const struct firmware *fw;
	const char *fw_name = "edgeport/down3.bin";

	/* In order to update the I2C firmware we must change the type 2 record
	 * to type 0xF2.  This will force the UMP to come up in Boot Mode.
	 * Then while in boot mode, the driver will download the latest
	 * firmware (padded to 15.5k) into the UMP ram.  And finally when the
	 * device comes back up in download mode the driver will cause the new
	 * firmware to be copied from the UMP Ram to I2C and the firmware will
	 * update the record type from 0xf2 to 0x02.
	 */

	/* Allocate a 15.5k buffer + 2 bytes for version number
	 * (Firmware Record) */
	buffer_size = (((1024 * 16) - 512 ) +
			sizeof(struct ti_i2c_firmware_rec));

	buffer = kmalloc(buffer_size, GFP_KERNEL);
	if (!buffer) {
		dev_err(dev, "%s - out of memory\n", __func__);
		return -ENOMEM;
	}

	memset(buffer, 0xff, buffer_size);

	err = request_firmware(&fw, fw_name, dev);
	if (err) {
		dev_err(dev, "Failed to load image \"%s\" err %d\n",
			fw_name, err);
		kfree(buffer);
		return err;
	}

	/* Save Download Version Number */
	OperationalMajorVersion = fw->data[0];
	OperationalMinorVersion = fw->data[1];
	OperationalBuildNumber = fw->data[2] | (fw->data[3] << 8);

	/* Copy version number into firmware record */
	firmware_rec = (struct ti_i2c_firmware_rec *)buffer;

	firmware_rec->Ver_Major	= OperationalMajorVersion;
	firmware_rec->Ver_Minor	= OperationalMinorVersion;

	/* Pointer to fw_down memory image */
	img_header = (struct ti_i2c_image_header *)&fw->data[4];

	memcpy(buffer + sizeof(struct ti_i2c_firmware_rec),
		&fw->data[4 + sizeof(struct ti_i2c_image_header)],
		le16_to_cpu(img_header->Length));

	release_firmware(fw);

	for (i=0; i < buffer_size; i++) {
		cs = (__u8)(cs + buffer[i]);
	}

	kfree(buffer);

	/* Build new header */
	i2c_header =  (struct ti_i2c_desc *)header;
	firmware_rec =  (struct ti_i2c_firmware_rec*)i2c_header->Data;

	i2c_header->Type	= I2C_DESC_TYPE_FIRMWARE_BLANK;
	i2c_header->Size	= (__u16)buffer_size;
	i2c_header->CheckSum	= cs;
	firmware_rec->Ver_Major	= OperationalMajorVersion;
	firmware_rec->Ver_Minor	= OperationalMinorVersion;

	return 0;
}
