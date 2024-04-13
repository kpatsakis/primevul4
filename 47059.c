static int virtnet_receive(struct receive_queue *rq, int budget)
{
	struct virtnet_info *vi = rq->vq->vdev->priv;
	unsigned int len, received = 0;
	void *buf;

	while (received < budget &&
	       (buf = virtqueue_get_buf(rq->vq, &len)) != NULL) {
		receive_buf(vi, rq, buf, len);
		received++;
	}

	if (rq->vq->num_free > virtqueue_get_vring_size(rq->vq) / 2) {
		if (!try_fill_recv(vi, rq, GFP_ATOMIC))
			schedule_delayed_work(&vi->refill, 0);
	}

	return received;
}
