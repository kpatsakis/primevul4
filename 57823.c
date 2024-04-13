static int check_fw_sanity(struct edgeport_serial *serial,
		const struct firmware *fw)
{
	u16 length_total;
	u8 checksum = 0;
	int pos;
	struct device *dev = &serial->serial->interface->dev;
	struct edgeport_fw_hdr *fw_hdr = (struct edgeport_fw_hdr *)fw->data;

	if (fw->size < sizeof(struct edgeport_fw_hdr)) {
		dev_err(dev, "incomplete fw header\n");
		return -EINVAL;
	}

	length_total = le16_to_cpu(fw_hdr->length) +
			sizeof(struct edgeport_fw_hdr);

	if (fw->size != length_total) {
		dev_err(dev, "bad fw size (expected: %u, got: %zu)\n",
				length_total, fw->size);
		return -EINVAL;
	}

	for (pos = sizeof(struct edgeport_fw_hdr); pos < fw->size; ++pos)
		checksum += fw->data[pos];

	if (checksum != fw_hdr->checksum) {
		dev_err(dev, "bad fw checksum (expected: 0x%x, got: 0x%x)\n",
				fw_hdr->checksum, checksum);
		return -EINVAL;
	}

	return 0;
}
