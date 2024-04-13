static int atl2_set_features(struct net_device *netdev,
	netdev_features_t features)
{
	netdev_features_t changed = netdev->features ^ features;

	if (changed & NETIF_F_HW_VLAN_CTAG_RX)
		atl2_vlan_mode(netdev, features);

	return 0;
}
