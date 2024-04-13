static int tpacket_fill_skb(struct packet_sock *po, struct sk_buff *skb,
		void *frame, struct net_device *dev, void *data, int tp_len,
		__be16 proto, unsigned char *addr, int hlen, int copylen,
		const struct sockcm_cookie *sockc)
{
	union tpacket_uhdr ph;
	int to_write, offset, len, nr_frags, len_max;
	struct socket *sock = po->sk.sk_socket;
	struct page *page;
	int err;

	ph.raw = frame;

	skb->protocol = proto;
	skb->dev = dev;
	skb->priority = po->sk.sk_priority;
	skb->mark = po->sk.sk_mark;
	sock_tx_timestamp(&po->sk, sockc->tsflags, &skb_shinfo(skb)->tx_flags);
	skb_shinfo(skb)->destructor_arg = ph.raw;

	skb_reserve(skb, hlen);
	skb_reset_network_header(skb);

	to_write = tp_len;

	if (sock->type == SOCK_DGRAM) {
		err = dev_hard_header(skb, dev, ntohs(proto), addr,
				NULL, tp_len);
		if (unlikely(err < 0))
			return -EINVAL;
	} else if (copylen) {
		int hdrlen = min_t(int, copylen, tp_len);

		skb_push(skb, dev->hard_header_len);
		skb_put(skb, copylen - dev->hard_header_len);
		err = skb_store_bits(skb, 0, data, hdrlen);
		if (unlikely(err))
			return err;
		if (!dev_validate_header(dev, skb->data, hdrlen))
			return -EINVAL;
		if (!skb->protocol)
			tpacket_set_protocol(dev, skb);

		data += hdrlen;
		to_write -= hdrlen;
	}

	offset = offset_in_page(data);
	len_max = PAGE_SIZE - offset;
	len = ((to_write > len_max) ? len_max : to_write);

	skb->data_len = to_write;
	skb->len += to_write;
	skb->truesize += to_write;
	atomic_add(to_write, &po->sk.sk_wmem_alloc);

	while (likely(to_write)) {
		nr_frags = skb_shinfo(skb)->nr_frags;

		if (unlikely(nr_frags >= MAX_SKB_FRAGS)) {
			pr_err("Packet exceed the number of skb frags(%lu)\n",
			       MAX_SKB_FRAGS);
			return -EFAULT;
		}

		page = pgv_to_page(data);
		data += len;
		flush_dcache_page(page);
		get_page(page);
		skb_fill_page_desc(skb, nr_frags, page, offset, len);
		to_write -= len;
		offset = 0;
		len_max = PAGE_SIZE;
		len = ((to_write > len_max) ? len_max : to_write);
	}

	skb_probe_transport_header(skb, 0);

	return tp_len;
}
