static void tg3_get_strings(struct net_device *dev, u32 stringset, u8 *buf)
{
	switch (stringset) {
	case ETH_SS_STATS:
		memcpy(buf, &ethtool_stats_keys, sizeof(ethtool_stats_keys));
		break;
	case ETH_SS_TEST:
		memcpy(buf, &ethtool_test_keys, sizeof(ethtool_test_keys));
		break;
	default:
		WARN_ON(1);	/* we need a WARN() */
		break;
	}
}
