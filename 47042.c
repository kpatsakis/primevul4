static int virtnet_alloc_queues(struct virtnet_info *vi)
{
	int i;

	vi->sq = kzalloc(sizeof(*vi->sq) * vi->max_queue_pairs, GFP_KERNEL);
	if (!vi->sq)
		goto err_sq;
	vi->rq = kzalloc(sizeof(*vi->rq) * vi->max_queue_pairs, GFP_KERNEL);
	if (!vi->rq)
		goto err_rq;

	INIT_DELAYED_WORK(&vi->refill, refill_work);
	for (i = 0; i < vi->max_queue_pairs; i++) {
		vi->rq[i].pages = NULL;
		netif_napi_add(vi->dev, &vi->rq[i].napi, virtnet_poll,
			       napi_weight);
		napi_hash_add(&vi->rq[i].napi);

		sg_init_table(vi->rq[i].sg, ARRAY_SIZE(vi->rq[i].sg));
		ewma_init(&vi->rq[i].mrg_avg_pkt_len, 1, RECEIVE_AVG_WEIGHT);
		sg_init_table(vi->sq[i].sg, ARRAY_SIZE(vi->sq[i].sg));
	}

	return 0;

err_rq:
	kfree(vi->sq);
err_sq:
	return -ENOMEM;
}
