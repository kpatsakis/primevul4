static int tg3_set_phys_id(struct net_device *dev,
			    enum ethtool_phys_id_state state)
{
	struct tg3 *tp = netdev_priv(dev);

	if (!netif_running(tp->dev))
		return -EAGAIN;

	switch (state) {
	case ETHTOOL_ID_ACTIVE:
		return 1;	/* cycle on/off once per second */

	case ETHTOOL_ID_ON:
		tw32(MAC_LED_CTRL, LED_CTRL_LNKLED_OVERRIDE |
		     LED_CTRL_1000MBPS_ON |
		     LED_CTRL_100MBPS_ON |
		     LED_CTRL_10MBPS_ON |
		     LED_CTRL_TRAFFIC_OVERRIDE |
		     LED_CTRL_TRAFFIC_BLINK |
		     LED_CTRL_TRAFFIC_LED);
		break;

	case ETHTOOL_ID_OFF:
		tw32(MAC_LED_CTRL, LED_CTRL_LNKLED_OVERRIDE |
		     LED_CTRL_TRAFFIC_OVERRIDE);
		break;

	case ETHTOOL_ID_INACTIVE:
		tw32(MAC_LED_CTRL, tp->led_ctrl);
		break;
	}

	return 0;
}
