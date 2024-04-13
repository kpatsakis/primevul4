static int tg3_close(struct net_device *dev)
{
	struct tg3 *tp = netdev_priv(dev);

	tg3_ptp_fini(tp);

	tg3_stop(tp);

	/* Clear stats across close / open calls */
	memset(&tp->net_stats_prev, 0, sizeof(tp->net_stats_prev));
	memset(&tp->estats_prev, 0, sizeof(tp->estats_prev));

	tg3_power_down(tp);

	tg3_carrier_off(tp);

	return 0;
}
