static int iboe_tos_to_sl(struct net_device *ndev, int tos)
{
	int prio;
	struct net_device *dev;

	prio = rt_tos2priority(tos);
	dev = ndev->priv_flags & IFF_802_1Q_VLAN ?
		vlan_dev_real_dev(ndev) : ndev;

	if (dev->num_tc)
		return netdev_get_prio_tc_map(dev, prio);

#if IS_ENABLED(CONFIG_VLAN_8021Q)
	if (ndev->priv_flags & IFF_802_1Q_VLAN)
		return (vlan_dev_get_egress_qos_mask(ndev, prio) &
			VLAN_PRIO_MASK) >> VLAN_PRIO_SHIFT;
#endif
	return 0;
}
