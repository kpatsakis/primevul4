static int tg3_set_mac_addr(struct net_device *dev, void *p)
{
	struct tg3 *tp = netdev_priv(dev);
	struct sockaddr *addr = p;
	int err = 0, skip_mac_1 = 0;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	if (!netif_running(dev))
		return 0;

	if (tg3_flag(tp, ENABLE_ASF)) {
		u32 addr0_high, addr0_low, addr1_high, addr1_low;

		addr0_high = tr32(MAC_ADDR_0_HIGH);
		addr0_low = tr32(MAC_ADDR_0_LOW);
		addr1_high = tr32(MAC_ADDR_1_HIGH);
		addr1_low = tr32(MAC_ADDR_1_LOW);

		/* Skip MAC addr 1 if ASF is using it. */
		if ((addr0_high != addr1_high || addr0_low != addr1_low) &&
		    !(addr1_high == 0 && addr1_low == 0))
			skip_mac_1 = 1;
	}
	spin_lock_bh(&tp->lock);
	__tg3_set_mac_addr(tp, skip_mac_1);
	spin_unlock_bh(&tp->lock);

	return err;
}
