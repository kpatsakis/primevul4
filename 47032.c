static void refill_work(struct work_struct *work)
{
	struct virtnet_info *vi =
		container_of(work, struct virtnet_info, refill.work);
	bool still_empty;
	int i;

	for (i = 0; i < vi->curr_queue_pairs; i++) {
		struct receive_queue *rq = &vi->rq[i];

		napi_disable(&rq->napi);
		still_empty = !try_fill_recv(vi, rq, GFP_KERNEL);
		virtnet_napi_enable(rq);

		/* In theory, this can happen: if we don't get any buffers in
		 * we will *never* try to fill again.
		 */
		if (still_empty)
			schedule_delayed_work(&vi->refill, HZ/2);
	}
}
