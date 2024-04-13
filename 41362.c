static int tg3_get_sset_count(struct net_device *dev, int sset)
{
	switch (sset) {
	case ETH_SS_TEST:
		return TG3_NUM_TEST;
	case ETH_SS_STATS:
		return TG3_NUM_STATS;
	default:
		return -EOPNOTSUPP;
	}
}
