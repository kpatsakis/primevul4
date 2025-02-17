static void tg3_poll_controller(struct net_device *dev)
{
	int i;
	struct tg3 *tp = netdev_priv(dev);

	if (tg3_irq_sync(tp))
		return;

	for (i = 0; i < tp->irq_cnt; i++)
		tg3_interrupt(tp->napi[i].irq_vec, &tp->napi[i]);
}
