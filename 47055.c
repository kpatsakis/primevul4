static void virtnet_napi_enable(struct receive_queue *rq)
{
	napi_enable(&rq->napi);

	/* If all buffers were filled by other side before we napi_enabled, we
	 * won't get another interrupt, so process any outstanding packets
	 * now.  virtnet_poll wants re-enable the queue, so we disable here.
	 * We synchronize against interrupts via NAPI_STATE_SCHED */
	if (napi_schedule_prep(&rq->napi)) {
		virtqueue_disable_cb(rq->vq);
		local_bh_disable();
		__napi_schedule(&rq->napi);
		local_bh_enable();
	}
}
