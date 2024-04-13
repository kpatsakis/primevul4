static int virtnet_close(struct net_device *dev)
{
	struct virtnet_info *vi = netdev_priv(dev);
	int i;

	/* Make sure refill_work doesn't re-enable napi! */
	cancel_delayed_work_sync(&vi->refill);

	for (i = 0; i < vi->max_queue_pairs; i++)
		napi_disable(&vi->rq[i].napi);

	return 0;
}
