static u8 tg3_resolve_flowctrl_1000X(u16 lcladv, u16 rmtadv)
{
	u8 cap = 0;

	if (lcladv & rmtadv & ADVERTISE_1000XPAUSE) {
		cap = FLOW_CTRL_TX | FLOW_CTRL_RX;
	} else if (lcladv & rmtadv & ADVERTISE_1000XPSE_ASYM) {
		if (lcladv & ADVERTISE_1000XPAUSE)
			cap = FLOW_CTRL_RX;
		if (rmtadv & ADVERTISE_1000XPAUSE)
			cap = FLOW_CTRL_TX;
	}

	return cap;
}
