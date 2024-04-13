static int packet_snd(struct socket *sock, struct msghdr *msg, size_t len)
{
	struct sock *sk = sock->sk;
	DECLARE_SOCKADDR(struct sockaddr_ll *, saddr, msg->msg_name);
	struct sk_buff *skb;
	struct net_device *dev;
	__be16 proto;
	unsigned char *addr;
	int err, reserve = 0;
	struct sockcm_cookie sockc;
	struct virtio_net_hdr vnet_hdr = { 0 };
	int offset = 0;
	struct packet_sock *po = pkt_sk(sk);
	int hlen, tlen, linear;
	int extra_len = 0;

	/*
	 *	Get and verify the address.
	 */

	if (likely(saddr == NULL)) {
		dev	= packet_cached_dev_get(po);
		proto	= po->num;
		addr	= NULL;
	} else {
		err = -EINVAL;
		if (msg->msg_namelen < sizeof(struct sockaddr_ll))
			goto out;
		if (msg->msg_namelen < (saddr->sll_halen + offsetof(struct sockaddr_ll, sll_addr)))
			goto out;
		proto	= saddr->sll_protocol;
		addr	= saddr->sll_addr;
		dev = dev_get_by_index(sock_net(sk), saddr->sll_ifindex);
	}

	err = -ENXIO;
	if (unlikely(dev == NULL))
		goto out_unlock;
	err = -ENETDOWN;
	if (unlikely(!(dev->flags & IFF_UP)))
		goto out_unlock;

	sockc.tsflags = sk->sk_tsflags;
	sockc.mark = sk->sk_mark;
	if (msg->msg_controllen) {
		err = sock_cmsg_send(sk, msg, &sockc);
		if (unlikely(err))
			goto out_unlock;
	}

	if (sock->type == SOCK_RAW)
		reserve = dev->hard_header_len;
	if (po->has_vnet_hdr) {
		err = packet_snd_vnet_parse(msg, &len, &vnet_hdr);
		if (err)
			goto out_unlock;
	}

	if (unlikely(sock_flag(sk, SOCK_NOFCS))) {
		if (!netif_supports_nofcs(dev)) {
			err = -EPROTONOSUPPORT;
			goto out_unlock;
		}
		extra_len = 4; /* We're doing our own CRC */
	}

	err = -EMSGSIZE;
	if (!vnet_hdr.gso_type &&
	    (len > dev->mtu + reserve + VLAN_HLEN + extra_len))
		goto out_unlock;

	err = -ENOBUFS;
	hlen = LL_RESERVED_SPACE(dev);
	tlen = dev->needed_tailroom;
	linear = __virtio16_to_cpu(vio_le(), vnet_hdr.hdr_len);
	linear = max(linear, min_t(int, len, dev->hard_header_len));
	skb = packet_alloc_skb(sk, hlen + tlen, hlen, len, linear,
			       msg->msg_flags & MSG_DONTWAIT, &err);
	if (skb == NULL)
		goto out_unlock;

	skb_set_network_header(skb, reserve);

	err = -EINVAL;
	if (sock->type == SOCK_DGRAM) {
		offset = dev_hard_header(skb, dev, ntohs(proto), addr, NULL, len);
		if (unlikely(offset < 0))
			goto out_free;
	}

	/* Returns -EFAULT on error */
	err = skb_copy_datagram_from_iter(skb, offset, &msg->msg_iter, len);
	if (err)
		goto out_free;

	if (sock->type == SOCK_RAW &&
	    !dev_validate_header(dev, skb->data, len)) {
		err = -EINVAL;
		goto out_free;
	}

	sock_tx_timestamp(sk, sockc.tsflags, &skb_shinfo(skb)->tx_flags);

	if (!vnet_hdr.gso_type && (len > dev->mtu + reserve + extra_len) &&
	    !packet_extra_vlan_len_allowed(dev, skb)) {
		err = -EMSGSIZE;
		goto out_free;
	}

	skb->protocol = proto;
	skb->dev = dev;
	skb->priority = sk->sk_priority;
	skb->mark = sockc.mark;

	if (po->has_vnet_hdr) {
		err = virtio_net_hdr_to_skb(skb, &vnet_hdr, vio_le());
		if (err)
			goto out_free;
		len += sizeof(vnet_hdr);
	}

	skb_probe_transport_header(skb, reserve);

	if (unlikely(extra_len == 4))
		skb->no_fcs = 1;

	err = po->xmit(skb);
	if (err > 0 && (err = net_xmit_errno(err)) != 0)
		goto out_unlock;

	dev_put(dev);

	return len;

out_free:
	kfree_skb(skb);
out_unlock:
	if (dev)
		dev_put(dev);
out:
	return err;
}
