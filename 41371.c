static int tg3_hwtstamp_ioctl(struct net_device *dev,
			      struct ifreq *ifr, int cmd)
{
	struct tg3 *tp = netdev_priv(dev);
	struct hwtstamp_config stmpconf;

	if (!tg3_flag(tp, PTP_CAPABLE))
		return -EINVAL;

	if (copy_from_user(&stmpconf, ifr->ifr_data, sizeof(stmpconf)))
		return -EFAULT;

	if (stmpconf.flags)
		return -EINVAL;

	switch (stmpconf.tx_type) {
	case HWTSTAMP_TX_ON:
		tg3_flag_set(tp, TX_TSTAMP_EN);
		break;
	case HWTSTAMP_TX_OFF:
		tg3_flag_clear(tp, TX_TSTAMP_EN);
		break;
	default:
		return -ERANGE;
	}

	switch (stmpconf.rx_filter) {
	case HWTSTAMP_FILTER_NONE:
		tp->rxptpctl = 0;
		break;
	case HWTSTAMP_FILTER_PTP_V1_L4_EVENT:
		tp->rxptpctl = TG3_RX_PTP_CTL_RX_PTP_V1_EN |
			       TG3_RX_PTP_CTL_ALL_V1_EVENTS;
		break;
	case HWTSTAMP_FILTER_PTP_V1_L4_SYNC:
		tp->rxptpctl = TG3_RX_PTP_CTL_RX_PTP_V1_EN |
			       TG3_RX_PTP_CTL_SYNC_EVNT;
		break;
	case HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ:
		tp->rxptpctl = TG3_RX_PTP_CTL_RX_PTP_V1_EN |
			       TG3_RX_PTP_CTL_DELAY_REQ;
		break;
	case HWTSTAMP_FILTER_PTP_V2_EVENT:
		tp->rxptpctl = TG3_RX_PTP_CTL_RX_PTP_V2_EN |
			       TG3_RX_PTP_CTL_ALL_V2_EVENTS;
		break;
	case HWTSTAMP_FILTER_PTP_V2_L2_EVENT:
		tp->rxptpctl = TG3_RX_PTP_CTL_RX_PTP_V2_L2_EN |
			       TG3_RX_PTP_CTL_ALL_V2_EVENTS;
		break;
	case HWTSTAMP_FILTER_PTP_V2_L4_EVENT:
		tp->rxptpctl = TG3_RX_PTP_CTL_RX_PTP_V2_L4_EN |
			       TG3_RX_PTP_CTL_ALL_V2_EVENTS;
		break;
	case HWTSTAMP_FILTER_PTP_V2_SYNC:
		tp->rxptpctl = TG3_RX_PTP_CTL_RX_PTP_V2_EN |
			       TG3_RX_PTP_CTL_SYNC_EVNT;
		break;
	case HWTSTAMP_FILTER_PTP_V2_L2_SYNC:
		tp->rxptpctl = TG3_RX_PTP_CTL_RX_PTP_V2_L2_EN |
			       TG3_RX_PTP_CTL_SYNC_EVNT;
		break;
	case HWTSTAMP_FILTER_PTP_V2_L4_SYNC:
		tp->rxptpctl = TG3_RX_PTP_CTL_RX_PTP_V2_L4_EN |
			       TG3_RX_PTP_CTL_SYNC_EVNT;
		break;
	case HWTSTAMP_FILTER_PTP_V2_DELAY_REQ:
		tp->rxptpctl = TG3_RX_PTP_CTL_RX_PTP_V2_EN |
			       TG3_RX_PTP_CTL_DELAY_REQ;
		break;
	case HWTSTAMP_FILTER_PTP_V2_L2_DELAY_REQ:
		tp->rxptpctl = TG3_RX_PTP_CTL_RX_PTP_V2_L2_EN |
			       TG3_RX_PTP_CTL_DELAY_REQ;
		break;
	case HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ:
		tp->rxptpctl = TG3_RX_PTP_CTL_RX_PTP_V2_L4_EN |
			       TG3_RX_PTP_CTL_DELAY_REQ;
		break;
	default:
		return -ERANGE;
	}

	if (netif_running(dev) && tp->rxptpctl)
		tw32(TG3_RX_PTP_CTL,
		     tp->rxptpctl | TG3_RX_PTP_CTL_HWTS_INTERLOCK);

	return copy_to_user(ifr->ifr_data, &stmpconf, sizeof(stmpconf)) ?
		-EFAULT : 0;
}
