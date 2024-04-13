fst_tx_timeout(struct net_device *dev)
{
	struct fst_port_info *port;
	struct fst_card_info *card;

	port = dev_to_port(dev);
	card = port->card;
	dev->stats.tx_errors++;
	dev->stats.tx_aborted_errors++;
	dbg(DBG_ASS, "Tx timeout card %d port %d\n",
	    card->card_no, port->index);
	fst_issue_cmd(port, ABORTTX);

	dev->trans_start = jiffies;
	netif_wake_queue(dev);
	port->start = 0;
}
