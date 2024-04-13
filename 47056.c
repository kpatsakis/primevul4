static void virtnet_netpoll(struct net_device *dev)
{
	struct virtnet_info *vi = netdev_priv(dev);
	int i;

	for (i = 0; i < vi->curr_queue_pairs; i++)
		napi_schedule(&vi->rq[i].napi);
}
