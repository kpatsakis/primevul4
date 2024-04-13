static int dgram_sendmsg(struct kiocb *iocb, struct sock *sk,
		struct msghdr *msg, size_t size)
{
	struct net_device *dev;
	unsigned int mtu;
	struct sk_buff *skb;
	struct dgram_sock *ro = dgram_sk(sk);
	int hlen, tlen;
	int err;

	if (msg->msg_flags & MSG_OOB) {
		pr_debug("msg->msg_flags = 0x%x\n", msg->msg_flags);
		return -EOPNOTSUPP;
	}

	if (!ro->bound)
		dev = dev_getfirstbyhwtype(sock_net(sk), ARPHRD_IEEE802154);
	else
		dev = ieee802154_get_dev(sock_net(sk), &ro->src_addr);

	if (!dev) {
		pr_debug("no dev\n");
		err = -ENXIO;
		goto out;
	}
	mtu = dev->mtu;
	pr_debug("name = %s, mtu = %u\n", dev->name, mtu);

	if (size > mtu) {
		pr_debug("size = %Zu, mtu = %u\n", size, mtu);
		err = -EINVAL;
		goto out_dev;
	}

	hlen = LL_RESERVED_SPACE(dev);
	tlen = dev->needed_tailroom;
	skb = sock_alloc_send_skb(sk, hlen + tlen + size,
			msg->msg_flags & MSG_DONTWAIT,
			&err);
	if (!skb)
		goto out_dev;

	skb_reserve(skb, hlen);

	skb_reset_network_header(skb);

	mac_cb(skb)->flags = IEEE802154_FC_TYPE_DATA;
	if (ro->want_ack)
		mac_cb(skb)->flags |= MAC_CB_FLAG_ACKREQ;

	mac_cb(skb)->seq = ieee802154_mlme_ops(dev)->get_dsn(dev);
	err = dev_hard_header(skb, dev, ETH_P_IEEE802154, &ro->dst_addr,
			ro->bound ? &ro->src_addr : NULL, size);
	if (err < 0)
		goto out_skb;

	skb_reset_mac_header(skb);

	err = memcpy_fromiovec(skb_put(skb, size), msg->msg_iov, size);
	if (err < 0)
		goto out_skb;

	skb->dev = dev;
	skb->sk  = sk;
	skb->protocol = htons(ETH_P_IEEE802154);

	dev_put(dev);

	err = dev_queue_xmit(skb);
	if (err > 0)
		err = net_xmit_errno(err);

	return err ?: size;

out_skb:
	kfree_skb(skb);
out_dev:
	dev_put(dev);
out:
	return err;
}
