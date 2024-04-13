static void tg3_tx_timeout(struct net_device *dev)
{
	struct tg3 *tp = netdev_priv(dev);

	if (netif_msg_tx_err(tp)) {
		netdev_err(dev, "transmit timed out, resetting\n");
		tg3_dump_state(tp);
	}

	tg3_reset_task_schedule(tp);
}
