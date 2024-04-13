static int wanxl_open(struct net_device *dev)
{
	port_t *port = dev_to_port(dev);
	u8 __iomem *dbr = port->card->plx + PLX_DOORBELL_TO_CARD;
	unsigned long timeout;
	int i;

	if (get_status(port)->open) {
		netdev_err(dev, "port already open\n");
		return -EIO;
	}
	if ((i = hdlc_open(dev)) != 0)
		return i;

	port->tx_in = port->tx_out = 0;
	for (i = 0; i < TX_BUFFERS; i++)
		get_status(port)->tx_descs[i].stat = PACKET_EMPTY;
	/* signal the card */
	writel(1 << (DOORBELL_TO_CARD_OPEN_0 + port->node), dbr);

	timeout = jiffies + HZ;
	do {
		if (get_status(port)->open) {
			netif_start_queue(dev);
			return 0;
		}
	} while (time_after(timeout, jiffies));

	netdev_err(dev, "unable to open port\n");
	/* ask the card to close the port, should it be still alive */
	writel(1 << (DOORBELL_TO_CARD_CLOSE_0 + port->node), dbr);
	return -EFAULT;
}
