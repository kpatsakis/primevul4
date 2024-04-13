static bool try_fill_recv(struct virtnet_info *vi, struct receive_queue *rq,
			  gfp_t gfp)
{
	int err;
	bool oom;

	gfp |= __GFP_COLD;
	do {
		if (vi->mergeable_rx_bufs)
			err = add_recvbuf_mergeable(rq, gfp);
		else if (vi->big_packets)
			err = add_recvbuf_big(vi, rq, gfp);
		else
			err = add_recvbuf_small(vi, rq, gfp);

		oom = err == -ENOMEM;
		if (err)
			break;
	} while (rq->vq->num_free);
	virtqueue_kick(rq->vq);
	return !oom;
}
