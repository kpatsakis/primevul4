fst_openport(struct fst_port_info *port)
{
	int signals;
	int txq_length;

	/* Only init things if card is actually running. This allows open to
	 * succeed for downloads etc.
	 */
	if (port->card->state == FST_RUNNING) {
		if (port->run) {
			dbg(DBG_OPEN, "open: found port already running\n");

			fst_issue_cmd(port, STOPPORT);
			port->run = 0;
		}

		fst_rx_config(port);
		fst_tx_config(port);
		fst_op_raise(port, OPSTS_RTS | OPSTS_DTR);

		fst_issue_cmd(port, STARTPORT);
		port->run = 1;

		signals = FST_RDL(port->card, v24DebouncedSts[port->index]);
		if (signals & (((port->hwif == X21) || (port->hwif == X21D))
			       ? IPSTS_INDICATE : IPSTS_DCD))
			netif_carrier_on(port_to_dev(port));
		else
			netif_carrier_off(port_to_dev(port));

		txq_length = port->txqe - port->txqs;
		port->txqe = 0;
		port->txqs = 0;
	}

}
