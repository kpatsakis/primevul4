fst_intr_te1_alarm(struct fst_card_info *card, struct fst_port_info *port)
{
	u8 los;
	u8 rra;
	u8 ais;

	los = FST_RDB(card, suStatus.lossOfSignal);
	rra = FST_RDB(card, suStatus.receiveRemoteAlarm);
	ais = FST_RDB(card, suStatus.alarmIndicationSignal);

	if (los) {
		/*
		 * Lost the link
		 */
		if (netif_carrier_ok(port_to_dev(port))) {
			dbg(DBG_INTR, "Net carrier off\n");
			netif_carrier_off(port_to_dev(port));
		}
	} else {
		/*
		 * Link available
		 */
		if (!netif_carrier_ok(port_to_dev(port))) {
			dbg(DBG_INTR, "Net carrier on\n");
			netif_carrier_on(port_to_dev(port));
		}
	}

	if (los)
		dbg(DBG_INTR, "Assert LOS Alarm\n");
	else
		dbg(DBG_INTR, "De-assert LOS Alarm\n");
	if (rra)
		dbg(DBG_INTR, "Assert RRA Alarm\n");
	else
		dbg(DBG_INTR, "De-assert RRA Alarm\n");

	if (ais)
		dbg(DBG_INTR, "Assert AIS Alarm\n");
	else
		dbg(DBG_INTR, "De-assert AIS Alarm\n");
}
