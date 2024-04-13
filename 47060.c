static int virtnet_restore(struct virtio_device *vdev)
{
	struct virtnet_info *vi = vdev->priv;
	int err, i;

	err = init_vqs(vi);
	if (err)
		return err;

	virtio_device_ready(vdev);

	if (netif_running(vi->dev)) {
		for (i = 0; i < vi->curr_queue_pairs; i++)
			if (!try_fill_recv(vi, &vi->rq[i], GFP_KERNEL))
				schedule_delayed_work(&vi->refill, 0);

		for (i = 0; i < vi->max_queue_pairs; i++)
			virtnet_napi_enable(&vi->rq[i]);
	}

	netif_device_attach(vi->dev);

	rtnl_lock();
	virtnet_set_queues(vi, vi->curr_queue_pairs);
	rtnl_unlock();

	err = register_hotcpu_notifier(&vi->nb);
	if (err)
		return err;

	return 0;
}
