static void cdc_ncm_get_ethtool_stats(struct net_device *netdev,
				    struct ethtool_stats __always_unused *stats,
				    u64 *data)
{
	struct usbnet *dev = netdev_priv(netdev);
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	int i;
	char *p = NULL;

	for (i = 0; i < ARRAY_SIZE(cdc_ncm_gstrings_stats); i++) {
		p = (char *)ctx + cdc_ncm_gstrings_stats[i].stat_offset;
		data[i] = (cdc_ncm_gstrings_stats[i].sizeof_stat == sizeof(u64)) ? *(u64 *)p : *(u32 *)p;
	}
}
