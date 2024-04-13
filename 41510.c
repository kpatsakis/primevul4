static inline void tg3_set_mtu(struct net_device *dev, struct tg3 *tp,
			       int new_mtu)
{
	dev->mtu = new_mtu;

	if (new_mtu > ETH_DATA_LEN) {
		if (tg3_flag(tp, 5780_CLASS)) {
			netdev_update_features(dev);
			tg3_flag_clear(tp, TSO_CAPABLE);
		} else {
			tg3_flag_set(tp, JUMBO_RING_ENABLE);
		}
	} else {
		if (tg3_flag(tp, 5780_CLASS)) {
			tg3_flag_set(tp, TSO_CAPABLE);
			netdev_update_features(dev);
		}
		tg3_flag_clear(tp, JUMBO_RING_ENABLE);
	}
}
