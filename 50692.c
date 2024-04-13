static int packet_sendmsg_spkt(struct socket *sock, struct msghdr *msg,
			       size_t len)
{
	struct sock *sk = sock->sk;
	DECLARE_SOCKADDR(struct sockaddr_pkt *, saddr, msg->msg_name);
	struct sk_buff *skb = NULL;
	struct net_device *dev;
	struct sockcm_cookie sockc;
	__be16 proto = 0;
	int err;
	int extra_len = 0;

	/*
	 *	Get and verify the address.
	 */

	if (saddr) {
		if (msg->msg_namelen < sizeof(struct sockaddr))
			return -EINVAL;
		if (msg->msg_namelen == sizeof(struct sockaddr_pkt))
			proto = saddr->spkt_protocol;
	} else
		return -ENOTCONN;	/* SOCK_PACKET must be sent giving an address */

	/*
	 *	Find the device first to size check it
	 */

	saddr->spkt_device[sizeof(saddr->spkt_device) - 1] = 0;
retry:
	rcu_read_lock();
	dev = dev_get_by_name_rcu(sock_net(sk), saddr->spkt_device);
	err = -ENODEV;
	if (dev == NULL)
		goto out_unlock;

	err = -ENETDOWN;
	if (!(dev->flags & IFF_UP))
		goto out_unlock;

	/*
	 * You may not queue a frame bigger than the mtu. This is the lowest level
	 * raw protocol and you must do your own fragmentation at this level.
	 */

	if (unlikely(sock_flag(sk, SOCK_NOFCS))) {
		if (!netif_supports_nofcs(dev)) {
			err = -EPROTONOSUPPORT;
			goto out_unlock;
		}
		extra_len = 4; /* We're doing our own CRC */
	}

	err = -EMSGSIZE;
	if (len > dev->mtu + dev->hard_header_len + VLAN_HLEN + extra_len)
		goto out_unlock;

	if (!skb) {
		size_t reserved = LL_RESERVED_SPACE(dev);
		int tlen = dev->needed_tailroom;
		unsigned int hhlen = dev->header_ops ? dev->hard_header_len : 0;

		rcu_read_unlock();
		skb = sock_wmalloc(sk, len + reserved + tlen, 0, GFP_KERNEL);
		if (skb == NULL)
			return -ENOBUFS;
		/* FIXME: Save some space for broken drivers that write a hard
		 * header at transmission time by themselves. PPP is the notable
		 * one here. This should really be fixed at the driver level.
		 */
		skb_reserve(skb, reserved);
		skb_reset_network_header(skb);

		/* Try to align data part correctly */
		if (hhlen) {
			skb->data -= hhlen;
			skb->tail -= hhlen;
			if (len < hhlen)
				skb_reset_network_header(skb);
		}
		err = memcpy_from_msg(skb_put(skb, len), msg, len);
		if (err)
			goto out_free;
		goto retry;
	}

	if (!dev_validate_header(dev, skb->data, len)) {
		err = -EINVAL;
		goto out_unlock;
	}
	if (len > (dev->mtu + dev->hard_header_len + extra_len) &&
	    !packet_extra_vlan_len_allowed(dev, skb)) {
		err = -EMSGSIZE;
		goto out_unlock;
	}

	sockc.tsflags = sk->sk_tsflags;
	if (msg->msg_controllen) {
		err = sock_cmsg_send(sk, msg, &sockc);
		if (unlikely(err))
			goto out_unlock;
	}

	skb->protocol = proto;
	skb->dev = dev;
	skb->priority = sk->sk_priority;
	skb->mark = sk->sk_mark;

	sock_tx_timestamp(sk, sockc.tsflags, &skb_shinfo(skb)->tx_flags);

	if (unlikely(extra_len == 4))
		skb->no_fcs = 1;

	skb_probe_transport_header(skb, 0);

	dev_queue_xmit(skb);
	rcu_read_unlock();
	return len;

out_unlock:
	rcu_read_unlock();
out_free:
	kfree_skb(skb);
	return err;
}
