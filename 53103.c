static int atl2_up(struct atl2_adapter *adapter)
{
	struct net_device *netdev = adapter->netdev;
	int err = 0;
	u32 val;

	/* hardware has been reset, we need to reload some things */

	err = atl2_init_hw(&adapter->hw);
	if (err) {
		err = -EIO;
		return err;
	}

	atl2_set_multi(netdev);
	init_ring_ptrs(adapter);

	atl2_restore_vlan(adapter);

	if (atl2_configure(adapter)) {
		err = -EIO;
		goto err_up;
	}

	clear_bit(__ATL2_DOWN, &adapter->flags);

	val = ATL2_READ_REG(&adapter->hw, REG_MASTER_CTRL);
	ATL2_WRITE_REG(&adapter->hw, REG_MASTER_CTRL, val |
		MASTER_CTRL_MANUAL_INT);

	atl2_irq_enable(adapter);

err_up:
	return err;
}
