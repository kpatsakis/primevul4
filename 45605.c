static int netlink_set_ring(struct sock *sk, struct nl_mmap_req *req,
			    bool closing, bool tx_ring)
{
	struct netlink_sock *nlk = nlk_sk(sk);
	struct netlink_ring *ring;
	struct sk_buff_head *queue;
	void **pg_vec = NULL;
	unsigned int order = 0;
	int err;

	ring  = tx_ring ? &nlk->tx_ring : &nlk->rx_ring;
	queue = tx_ring ? &sk->sk_write_queue : &sk->sk_receive_queue;

	if (!closing) {
		if (atomic_read(&nlk->mapped))
			return -EBUSY;
		if (atomic_read(&ring->pending))
			return -EBUSY;
	}

	if (req->nm_block_nr) {
		if (ring->pg_vec != NULL)
			return -EBUSY;

		if ((int)req->nm_block_size <= 0)
			return -EINVAL;
		if (!IS_ALIGNED(req->nm_block_size, PAGE_SIZE))
			return -EINVAL;
		if (req->nm_frame_size < NL_MMAP_HDRLEN)
			return -EINVAL;
		if (!IS_ALIGNED(req->nm_frame_size, NL_MMAP_MSG_ALIGNMENT))
			return -EINVAL;

		ring->frames_per_block = req->nm_block_size /
					 req->nm_frame_size;
		if (ring->frames_per_block == 0)
			return -EINVAL;
		if (ring->frames_per_block * req->nm_block_nr !=
		    req->nm_frame_nr)
			return -EINVAL;

		order = get_order(req->nm_block_size);
		pg_vec = alloc_pg_vec(nlk, req, order);
		if (pg_vec == NULL)
			return -ENOMEM;
	} else {
		if (req->nm_frame_nr)
			return -EINVAL;
	}

	err = -EBUSY;
	mutex_lock(&nlk->pg_vec_lock);
	if (closing || atomic_read(&nlk->mapped) == 0) {
		err = 0;
		spin_lock_bh(&queue->lock);

		ring->frame_max		= req->nm_frame_nr - 1;
		ring->head		= 0;
		ring->frame_size	= req->nm_frame_size;
		ring->pg_vec_pages	= req->nm_block_size / PAGE_SIZE;

		swap(ring->pg_vec_len, req->nm_block_nr);
		swap(ring->pg_vec_order, order);
		swap(ring->pg_vec, pg_vec);

		__skb_queue_purge(queue);
		spin_unlock_bh(&queue->lock);

		WARN_ON(atomic_read(&nlk->mapped));
	}
	mutex_unlock(&nlk->pg_vec_lock);

	if (pg_vec)
		free_pg_vec(pg_vec, order, req->nm_block_nr);
	return err;
}
