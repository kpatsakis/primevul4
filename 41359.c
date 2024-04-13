static u32 tg3_get_rxfh_indir_size(struct net_device *dev)
{
	u32 size = 0;
	struct tg3 *tp = netdev_priv(dev);

	if (tg3_flag(tp, SUPPORT_MSIX))
		size = TG3_RSS_INDIR_TBL_SIZE;

	return size;
}
