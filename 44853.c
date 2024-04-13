fst_set_iface(struct fst_card_info *card, struct fst_port_info *port,
	      struct ifreq *ifr)
{
	sync_serial_settings sync;
	int i;

	if (ifr->ifr_settings.size != sizeof (sync)) {
		return -ENOMEM;
	}

	if (copy_from_user
	    (&sync, ifr->ifr_settings.ifs_ifsu.sync, sizeof (sync))) {
		return -EFAULT;
	}

	if (sync.loopback)
		return -EINVAL;

	i = port->index;

	switch (ifr->ifr_settings.type) {
	case IF_IFACE_V35:
		FST_WRW(card, portConfig[i].lineInterface, V35);
		port->hwif = V35;
		break;

	case IF_IFACE_V24:
		FST_WRW(card, portConfig[i].lineInterface, V24);
		port->hwif = V24;
		break;

	case IF_IFACE_X21:
		FST_WRW(card, portConfig[i].lineInterface, X21);
		port->hwif = X21;
		break;

	case IF_IFACE_X21D:
		FST_WRW(card, portConfig[i].lineInterface, X21D);
		port->hwif = X21D;
		break;

	case IF_IFACE_T1:
		FST_WRW(card, portConfig[i].lineInterface, T1);
		port->hwif = T1;
		break;

	case IF_IFACE_E1:
		FST_WRW(card, portConfig[i].lineInterface, E1);
		port->hwif = E1;
		break;

	case IF_IFACE_SYNC_SERIAL:
		break;

	default:
		return -EINVAL;
	}

	switch (sync.clock_type) {
	case CLOCK_EXT:
		FST_WRB(card, portConfig[i].internalClock, EXTCLK);
		break;

	case CLOCK_INT:
		FST_WRB(card, portConfig[i].internalClock, INTCLK);
		break;

	default:
		return -EINVAL;
	}
	FST_WRL(card, portConfig[i].lineSpeed, sync.clock_rate);
	return 0;
}
