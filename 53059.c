static void atl2_get_wol(struct net_device *netdev,
	struct ethtool_wolinfo *wol)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);

	wol->supported = WAKE_MAGIC;
	wol->wolopts = 0;

	if (adapter->wol & ATLX_WUFC_EX)
		wol->wolopts |= WAKE_UCAST;
	if (adapter->wol & ATLX_WUFC_MC)
		wol->wolopts |= WAKE_MCAST;
	if (adapter->wol & ATLX_WUFC_BC)
		wol->wolopts |= WAKE_BCAST;
	if (adapter->wol & ATLX_WUFC_MAG)
		wol->wolopts |= WAKE_MAGIC;
	if (adapter->wol & ATLX_WUFC_LNKC)
		wol->wolopts |= WAKE_PHY;
}
