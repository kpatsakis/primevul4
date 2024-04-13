static int tg3_get_rxfh_indir(struct net_device *dev, u32 *indir)
{
	struct tg3 *tp = netdev_priv(dev);
	int i;

	for (i = 0; i < TG3_RSS_INDIR_TBL_SIZE; i++)
		indir[i] = tp->rss_ind_tbl[i];

	return 0;
}
