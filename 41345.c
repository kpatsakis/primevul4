static int tg3_get_eeprom_len(struct net_device *dev)
{
	struct tg3 *tp = netdev_priv(dev);

	return tp->nvram_size;
}
