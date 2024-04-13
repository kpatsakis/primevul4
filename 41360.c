static int tg3_get_rxnfc(struct net_device *dev, struct ethtool_rxnfc *info,
			 u32 *rules __always_unused)
{
	struct tg3 *tp = netdev_priv(dev);

	if (!tg3_flag(tp, SUPPORT_MSIX))
		return -EOPNOTSUPP;

	switch (info->cmd) {
	case ETHTOOL_GRXRINGS:
		if (netif_running(tp->dev))
			info->data = tp->rxq_cnt;
		else {
			info->data = num_online_cpus();
			if (info->data > TG3_RSS_MAX_NUM_QS)
				info->data = TG3_RSS_MAX_NUM_QS;
		}

		/* The first interrupt vector only
		 * handles link interrupts.
		 */
		info->data -= 1;
		return 0;

	default:
		return -EOPNOTSUPP;
	}
}
