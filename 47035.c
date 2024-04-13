static void skb_recv_done(struct virtqueue *rvq)
{
	struct virtnet_info *vi = rvq->vdev->priv;
	struct receive_queue *rq = &vi->rq[vq2rxq(rvq)];

	/* Schedule NAPI, Suppress further interrupts if successful. */
	if (napi_schedule_prep(&rq->napi)) {
		virtqueue_disable_cb(rvq);
		__napi_schedule(&rq->napi);
	}
}
