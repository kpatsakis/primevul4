static void virtnet_free_queues(struct virtnet_info *vi)
{
	int i;

	for (i = 0; i < vi->max_queue_pairs; i++) {
		napi_hash_del(&vi->rq[i].napi);
		netif_napi_del(&vi->rq[i].napi);
	}

	kfree(vi->rq);
	kfree(vi->sq);
}
