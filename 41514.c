static int tg3_set_rxfh_indir(struct net_device *dev, const u32 *indir)
{
	struct tg3 *tp = netdev_priv(dev);
	size_t i;

	for (i = 0; i < TG3_RSS_INDIR_TBL_SIZE; i++)
		tp->rss_ind_tbl[i] = indir[i];

	if (!netif_running(dev) || !tg3_flag(tp, ENABLE_RSS))
		return 0;

	/* It is legal to write the indirection
	 * table while the device is running.
	 */
	tg3_full_lock(tp, 0);
	tg3_rss_write_indir_tbl(tp);
	tg3_full_unlock(tp);

	return 0;
}
