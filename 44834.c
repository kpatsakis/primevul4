fst_intr_ctlchg(struct fst_card_info *card, struct fst_port_info *port)
{
	int signals;

	signals = FST_RDL(card, v24DebouncedSts[port->index]);

	if (signals & (((port->hwif == X21) || (port->hwif == X21D))
		       ? IPSTS_INDICATE : IPSTS_DCD)) {
		if (!netif_carrier_ok(port_to_dev(port))) {
			dbg(DBG_INTR, "DCD active\n");
			netif_carrier_on(port_to_dev(port));
		}
	} else {
		if (netif_carrier_ok(port_to_dev(port))) {
			dbg(DBG_INTR, "DCD lost\n");
			netif_carrier_off(port_to_dev(port));
		}
	}
}
