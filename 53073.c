static int atl2_mii_ioctl(struct net_device *netdev, struct ifreq *ifr, int cmd)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);
	struct mii_ioctl_data *data = if_mii(ifr);
	unsigned long flags;

	switch (cmd) {
	case SIOCGMIIPHY:
		data->phy_id = 0;
		break;
	case SIOCGMIIREG:
		spin_lock_irqsave(&adapter->stats_lock, flags);
		if (atl2_read_phy_reg(&adapter->hw,
			data->reg_num & 0x1F, &data->val_out)) {
			spin_unlock_irqrestore(&adapter->stats_lock, flags);
			return -EIO;
		}
		spin_unlock_irqrestore(&adapter->stats_lock, flags);
		break;
	case SIOCSMIIREG:
		if (data->reg_num & ~(0x1F))
			return -EFAULT;
		spin_lock_irqsave(&adapter->stats_lock, flags);
		if (atl2_write_phy_reg(&adapter->hw, data->reg_num,
			data->val_in)) {
			spin_unlock_irqrestore(&adapter->stats_lock, flags);
			return -EIO;
		}
		spin_unlock_irqrestore(&adapter->stats_lock, flags);
		break;
	default:
		return -EOPNOTSUPP;
	}
	return 0;
}
