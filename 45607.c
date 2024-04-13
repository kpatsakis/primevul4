static void netlink_skb_destructor(struct sk_buff *skb)
{
#ifdef CONFIG_NETLINK_MMAP
	struct nl_mmap_hdr *hdr;
	struct netlink_ring *ring;
	struct sock *sk;

	/* If a packet from the kernel to userspace was freed because of an
	 * error without being delivered to userspace, the kernel must reset
	 * the status. In the direction userspace to kernel, the status is
	 * always reset here after the packet was processed and freed.
	 */
	if (netlink_skb_is_mmaped(skb)) {
		hdr = netlink_mmap_hdr(skb);
		sk = NETLINK_CB(skb).sk;

		if (NETLINK_CB(skb).flags & NETLINK_SKB_TX) {
			netlink_set_status(hdr, NL_MMAP_STATUS_UNUSED);
			ring = &nlk_sk(sk)->tx_ring;
		} else {
			if (!(NETLINK_CB(skb).flags & NETLINK_SKB_DELIVERED)) {
				hdr->nm_len = 0;
				netlink_set_status(hdr, NL_MMAP_STATUS_VALID);
			}
			ring = &nlk_sk(sk)->rx_ring;
		}

		WARN_ON(atomic_read(&ring->pending) == 0);
		atomic_dec(&ring->pending);
		sock_put(sk);

		skb->head = NULL;
	}
#endif
	if (is_vmalloc_addr(skb->head)) {
		if (!skb->cloned ||
		    !atomic_dec_return(&(skb_shinfo(skb)->dataref)))
			vfree(skb->head);

		skb->head = NULL;
	}
	if (skb->sk != NULL)
		sock_rfree(skb);
}
