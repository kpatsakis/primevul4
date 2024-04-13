static int virtnet_set_queues(struct virtnet_info *vi, u16 queue_pairs)
{
	struct scatterlist sg;
	struct virtio_net_ctrl_mq s;
	struct net_device *dev = vi->dev;

	if (!vi->has_cvq || !virtio_has_feature(vi->vdev, VIRTIO_NET_F_MQ))
		return 0;

	s.virtqueue_pairs = cpu_to_virtio16(vi->vdev, queue_pairs);
	sg_init_one(&sg, &s, sizeof(s));

	if (!virtnet_send_command(vi, VIRTIO_NET_CTRL_MQ,
				  VIRTIO_NET_CTRL_MQ_VQ_PAIRS_SET, &sg)) {
		dev_warn(&dev->dev, "Fail to set num of queue pairs to %d\n",
			 queue_pairs);
		return -EINVAL;
	} else {
		vi->curr_queue_pairs = queue_pairs;
		/* virtnet_open() will refill when device is going to up. */
		if (dev->flags & IFF_UP)
			schedule_delayed_work(&vi->refill, 0);
	}

	return 0;
}
