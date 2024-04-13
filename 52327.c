static void cdc_ncm_get_strings(struct net_device __always_unused *netdev, u32 stringset, u8 *data)
{
	u8 *p = data;
	int i;

	switch (stringset) {
	case ETH_SS_STATS:
		for (i = 0; i < ARRAY_SIZE(cdc_ncm_gstrings_stats); i++) {
			memcpy(p, cdc_ncm_gstrings_stats[i].stat_string, ETH_GSTRING_LEN);
			p += ETH_GSTRING_LEN;
		}
	}
}
