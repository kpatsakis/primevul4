static int virtnet_freeze(struct virtio_device *vdev)
{
	struct virtnet_info *vi = vdev->priv;
	int i;

	unregister_hotcpu_notifier(&vi->nb);

	/* Make sure no work handler is accessing the device */
	flush_work(&vi->config_work);

	netif_device_detach(vi->dev);
	cancel_delayed_work_sync(&vi->refill);

	if (netif_running(vi->dev)) {
		for (i = 0; i < vi->max_queue_pairs; i++)
			napi_disable(&vi->rq[i].napi);
	}

	remove_vq_common(vi);

	return 0;
}
