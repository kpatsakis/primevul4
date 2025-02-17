static int tg3_get_eeprom(struct net_device *dev, struct ethtool_eeprom *eeprom, u8 *data)
{
	struct tg3 *tp = netdev_priv(dev);
	int ret;
	u8  *pd;
	u32 i, offset, len, b_offset, b_count;
	__be32 val;

	if (tg3_flag(tp, NO_NVRAM))
		return -EINVAL;

	if (tp->phy_flags & TG3_PHYFLG_IS_LOW_POWER)
		return -EAGAIN;

	offset = eeprom->offset;
	len = eeprom->len;
	eeprom->len = 0;

	eeprom->magic = TG3_EEPROM_MAGIC;

	if (offset & 3) {
		/* adjustments to start on required 4 byte boundary */
		b_offset = offset & 3;
		b_count = 4 - b_offset;
		if (b_count > len) {
			/* i.e. offset=1 len=2 */
			b_count = len;
		}
		ret = tg3_nvram_read_be32(tp, offset-b_offset, &val);
		if (ret)
			return ret;
		memcpy(data, ((char *)&val) + b_offset, b_count);
		len -= b_count;
		offset += b_count;
		eeprom->len += b_count;
	}

	/* read bytes up to the last 4 byte boundary */
	pd = &data[eeprom->len];
	for (i = 0; i < (len - (len & 3)); i += 4) {
		ret = tg3_nvram_read_be32(tp, offset + i, &val);
		if (ret) {
			eeprom->len += i;
			return ret;
		}
		memcpy(pd + i, &val, 4);
	}
	eeprom->len += i;

	if (len & 3) {
		/* read last bytes not ending on 4 byte boundary */
		pd = &data[eeprom->len];
		b_count = len & 3;
		b_offset = offset + len - b_count;
		ret = tg3_nvram_read_be32(tp, b_offset, &val);
		if (ret)
			return ret;
		memcpy(pd, &val, b_count);
		eeprom->len += b_count;
	}
	return 0;
}
