static int cdc_ncm_get_sset_count(struct net_device __always_unused *netdev, int sset)
{
	switch (sset) {
	case ETH_SS_STATS:
		return ARRAY_SIZE(cdc_ncm_gstrings_stats);
	default:
		return -EOPNOTSUPP;
	}
}
