static int tg3_get_default_macaddr_sparc(struct tg3 *tp)
{
	struct net_device *dev = tp->dev;

	memcpy(dev->dev_addr, idprom->id_ethaddr, 6);
	return 0;
}
