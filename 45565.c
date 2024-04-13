struct sk_buff *netlink_alloc_skb(struct sock *ssk, unsigned int size,
				  u32 dst_portid, gfp_t gfp_mask)
{
#ifdef CONFIG_NETLINK_MMAP
	struct sock *sk = NULL;
	struct sk_buff *skb;
	struct netlink_ring *ring;
	struct nl_mmap_hdr *hdr;
	unsigned int maxlen;

	sk = netlink_getsockbyportid(ssk, dst_portid);
	if (IS_ERR(sk))
		goto out;

	ring = &nlk_sk(sk)->rx_ring;
	/* fast-path without atomic ops for common case: non-mmaped receiver */
	if (ring->pg_vec == NULL)
		goto out_put;

	skb = alloc_skb_head(gfp_mask);
	if (skb == NULL)
		goto err1;

	spin_lock_bh(&sk->sk_receive_queue.lock);
	/* check again under lock */
	if (ring->pg_vec == NULL)
		goto out_free;

	maxlen = ring->frame_size - NL_MMAP_HDRLEN;
	if (maxlen < size)
		goto out_free;

	netlink_forward_ring(ring);
	hdr = netlink_current_frame(ring, NL_MMAP_STATUS_UNUSED);
	if (hdr == NULL)
		goto err2;
	netlink_ring_setup_skb(skb, sk, ring, hdr);
	netlink_set_status(hdr, NL_MMAP_STATUS_RESERVED);
	atomic_inc(&ring->pending);
	netlink_increment_head(ring);

	spin_unlock_bh(&sk->sk_receive_queue.lock);
	return skb;

err2:
	kfree_skb(skb);
	spin_unlock_bh(&sk->sk_receive_queue.lock);
	netlink_overrun(sk);
err1:
	sock_put(sk);
	return NULL;

out_free:
	kfree_skb(skb);
	spin_unlock_bh(&sk->sk_receive_queue.lock);
out_put:
	sock_put(sk);
out:
#endif
	return alloc_skb(size, gfp_mask);
}
