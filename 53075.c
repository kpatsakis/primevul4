static int atl2_open(struct net_device *netdev)
{
	struct atl2_adapter *adapter = netdev_priv(netdev);
	int err;
	u32 val;

	/* disallow open during test */
	if (test_bit(__ATL2_TESTING, &adapter->flags))
		return -EBUSY;

	/* allocate transmit descriptors */
	err = atl2_setup_ring_resources(adapter);
	if (err)
		return err;

	err = atl2_init_hw(&adapter->hw);
	if (err) {
		err = -EIO;
		goto err_init_hw;
	}

	/* hardware has been reset, we need to reload some things */
	atl2_set_multi(netdev);
	init_ring_ptrs(adapter);

	atl2_restore_vlan(adapter);

	if (atl2_configure(adapter)) {
		err = -EIO;
		goto err_config;
	}

	err = atl2_request_irq(adapter);
	if (err)
		goto err_req_irq;

	clear_bit(__ATL2_DOWN, &adapter->flags);

	mod_timer(&adapter->watchdog_timer, round_jiffies(jiffies + 4*HZ));

	val = ATL2_READ_REG(&adapter->hw, REG_MASTER_CTRL);
	ATL2_WRITE_REG(&adapter->hw, REG_MASTER_CTRL,
		val | MASTER_CTRL_MANUAL_INT);

	atl2_irq_enable(adapter);

	return 0;

err_init_hw:
err_req_irq:
err_config:
	atl2_free_ring_resources(adapter);
	atl2_reset_hw(&adapter->hw);

	return err;
}
