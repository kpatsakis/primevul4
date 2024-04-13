static int netlink_mmap_sendmsg(struct sock *sk, struct msghdr *msg,
				u32 dst_portid, u32 dst_group,
				struct sock_iocb *siocb)
{
	struct netlink_sock *nlk = nlk_sk(sk);
	struct netlink_ring *ring;
	struct nl_mmap_hdr *hdr;
	struct sk_buff *skb;
	unsigned int maxlen;
	bool excl = true;
	int err = 0, len = 0;

	/* Netlink messages are validated by the receiver before processing.
	 * In order to avoid userspace changing the contents of the message
	 * after validation, the socket and the ring may only be used by a
	 * single process, otherwise we fall back to copying.
	 */
	if (atomic_long_read(&sk->sk_socket->file->f_count) > 2 ||
	    atomic_read(&nlk->mapped) > 1)
		excl = false;

	mutex_lock(&nlk->pg_vec_lock);

	ring   = &nlk->tx_ring;
	maxlen = ring->frame_size - NL_MMAP_HDRLEN;

	do {
		hdr = netlink_current_frame(ring, NL_MMAP_STATUS_VALID);
		if (hdr == NULL) {
			if (!(msg->msg_flags & MSG_DONTWAIT) &&
			    atomic_read(&nlk->tx_ring.pending))
				schedule();
			continue;
		}
		if (hdr->nm_len > maxlen) {
			err = -EINVAL;
			goto out;
		}

		netlink_frame_flush_dcache(hdr);

		if (likely(dst_portid == 0 && dst_group == 0 && excl)) {
			skb = alloc_skb_head(GFP_KERNEL);
			if (skb == NULL) {
				err = -ENOBUFS;
				goto out;
			}
			sock_hold(sk);
			netlink_ring_setup_skb(skb, sk, ring, hdr);
			NETLINK_CB(skb).flags |= NETLINK_SKB_TX;
			__skb_put(skb, hdr->nm_len);
			netlink_set_status(hdr, NL_MMAP_STATUS_RESERVED);
			atomic_inc(&ring->pending);
		} else {
			skb = alloc_skb(hdr->nm_len, GFP_KERNEL);
			if (skb == NULL) {
				err = -ENOBUFS;
				goto out;
			}
			__skb_put(skb, hdr->nm_len);
			memcpy(skb->data, (void *)hdr + NL_MMAP_HDRLEN, hdr->nm_len);
			netlink_set_status(hdr, NL_MMAP_STATUS_UNUSED);
		}

		netlink_increment_head(ring);

		NETLINK_CB(skb).portid	  = nlk->portid;
		NETLINK_CB(skb).dst_group = dst_group;
		NETLINK_CB(skb).creds	  = siocb->scm->creds;

		err = security_netlink_send(sk, skb);
		if (err) {
			kfree_skb(skb);
			goto out;
		}

		if (unlikely(dst_group)) {
			atomic_inc(&skb->users);
			netlink_broadcast(sk, skb, dst_portid, dst_group,
					  GFP_KERNEL);
		}
		err = netlink_unicast(sk, skb, dst_portid,
				      msg->msg_flags & MSG_DONTWAIT);
		if (err < 0)
			goto out;
		len += err;

	} while (hdr != NULL ||
		 (!(msg->msg_flags & MSG_DONTWAIT) &&
		  atomic_read(&nlk->tx_ring.pending)));

	if (len > 0)
		err = len;
out:
	mutex_unlock(&nlk->pg_vec_lock);
	return err;
}
