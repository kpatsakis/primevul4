aiptek_query(struct aiptek *aiptek, unsigned char command, unsigned char data)
{
	const int sizeof_buf = 3 * sizeof(u8);
	int ret;
	u8 *buf;

	buf = kmalloc(sizeof_buf, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	buf[0] = 2;
	buf[1] = command;
	buf[2] = data;

	if (aiptek_command(aiptek, command, data) != 0) {
		kfree(buf);
		return -EIO;
	}
	msleep(aiptek->curSetting.programmableDelay);

	if ((ret =
	     aiptek_get_report(aiptek, 3, 2, buf, sizeof_buf)) != sizeof_buf) {
		dev_dbg(&aiptek->intf->dev,
			"aiptek_query failed: returned 0x%02x 0x%02x 0x%02x\n",
			buf[0], buf[1], buf[2]);
		ret = -EIO;
	} else {
		ret = get_unaligned_le16(buf + 1);
	}
	kfree(buf);
	return ret;
}
