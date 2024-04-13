static int virtnet_poll(struct napi_struct *napi, int budget)
{
	struct receive_queue *rq =
		container_of(napi, struct receive_queue, napi);
	unsigned int r, received;

	received = virtnet_receive(rq, budget);

	/* Out of packets? */
	if (received < budget) {
		r = virtqueue_enable_cb_prepare(rq->vq);
		napi_complete(napi);
		if (unlikely(virtqueue_poll(rq->vq, r)) &&
		    napi_schedule_prep(napi)) {
			virtqueue_disable_cb(rq->vq);
			__napi_schedule(napi);
		}
	}

	return received;
}
