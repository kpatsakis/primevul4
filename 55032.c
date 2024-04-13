static int tpacket_snd(struct packet_sock *po, struct msghdr *msg)
{
	struct sk_buff *skb;
	struct net_device *dev;
	struct virtio_net_hdr *vnet_hdr = NULL;
	struct sockcm_cookie sockc;
	__be16 proto;
	int err, reserve = 0;
	void *ph;
	DECLARE_SOCKADDR(struct sockaddr_ll *, saddr, msg->msg_name);
	bool need_wait = !(msg->msg_flags & MSG_DONTWAIT);
	int tp_len, size_max;
	unsigned char *addr;
	void *data;
	int len_sum = 0;
	int status = TP_STATUS_AVAILABLE;
	int hlen, tlen, copylen = 0;

	mutex_lock(&po->pg_vec_lock);

	if (likely(saddr == NULL)) {
		dev	= packet_cached_dev_get(po);
		proto	= po->num;
		addr	= NULL;
	} else {
		err = -EINVAL;
		if (msg->msg_namelen < sizeof(struct sockaddr_ll))
			goto out;
		if (msg->msg_namelen < (saddr->sll_halen
					+ offsetof(struct sockaddr_ll,
						sll_addr)))
			goto out;
		proto	= saddr->sll_protocol;
		addr	= saddr->sll_addr;
		dev = dev_get_by_index(sock_net(&po->sk), saddr->sll_ifindex);
	}

	err = -ENXIO;
	if (unlikely(dev == NULL))
		goto out;
	err = -ENETDOWN;
	if (unlikely(!(dev->flags & IFF_UP)))
		goto out_put;

	sockc.tsflags = po->sk.sk_tsflags;
	if (msg->msg_controllen) {
		err = sock_cmsg_send(&po->sk, msg, &sockc);
		if (unlikely(err))
			goto out_put;
	}

	if (po->sk.sk_socket->type == SOCK_RAW)
		reserve = dev->hard_header_len;
	size_max = po->tx_ring.frame_size
		- (po->tp_hdrlen - sizeof(struct sockaddr_ll));

	if ((size_max > dev->mtu + reserve + VLAN_HLEN) && !po->has_vnet_hdr)
		size_max = dev->mtu + reserve + VLAN_HLEN;

	do {
		ph = packet_current_frame(po, &po->tx_ring,
					  TP_STATUS_SEND_REQUEST);
		if (unlikely(ph == NULL)) {
			if (need_wait && need_resched())
				schedule();
			continue;
		}

		skb = NULL;
		tp_len = tpacket_parse_header(po, ph, size_max, &data);
		if (tp_len < 0)
			goto tpacket_error;

		status = TP_STATUS_SEND_REQUEST;
		hlen = LL_RESERVED_SPACE(dev);
		tlen = dev->needed_tailroom;
		if (po->has_vnet_hdr) {
			vnet_hdr = data;
			data += sizeof(*vnet_hdr);
			tp_len -= sizeof(*vnet_hdr);
			if (tp_len < 0 ||
			    __packet_snd_vnet_parse(vnet_hdr, tp_len)) {
				tp_len = -EINVAL;
				goto tpacket_error;
			}
			copylen = __virtio16_to_cpu(vio_le(),
						    vnet_hdr->hdr_len);
		}
		copylen = max_t(int, copylen, dev->hard_header_len);
		skb = sock_alloc_send_skb(&po->sk,
				hlen + tlen + sizeof(struct sockaddr_ll) +
				(copylen - dev->hard_header_len),
				!need_wait, &err);

		if (unlikely(skb == NULL)) {
			/* we assume the socket was initially writeable ... */
			if (likely(len_sum > 0))
				err = len_sum;
			goto out_status;
		}
		tp_len = tpacket_fill_skb(po, skb, ph, dev, data, tp_len, proto,
					  addr, hlen, copylen, &sockc);
		if (likely(tp_len >= 0) &&
		    tp_len > dev->mtu + reserve &&
		    !po->has_vnet_hdr &&
		    !packet_extra_vlan_len_allowed(dev, skb))
			tp_len = -EMSGSIZE;

		if (unlikely(tp_len < 0)) {
tpacket_error:
			if (po->tp_loss) {
				__packet_set_status(po, ph,
						TP_STATUS_AVAILABLE);
				packet_increment_head(&po->tx_ring);
				kfree_skb(skb);
				continue;
			} else {
				status = TP_STATUS_WRONG_FORMAT;
				err = tp_len;
				goto out_status;
			}
		}

		if (po->has_vnet_hdr && virtio_net_hdr_to_skb(skb, vnet_hdr,
							      vio_le())) {
			tp_len = -EINVAL;
			goto tpacket_error;
		}

		skb->destructor = tpacket_destruct_skb;
		__packet_set_status(po, ph, TP_STATUS_SENDING);
		packet_inc_pending(&po->tx_ring);

		status = TP_STATUS_SEND_REQUEST;
		err = po->xmit(skb);
		if (unlikely(err > 0)) {
			err = net_xmit_errno(err);
			if (err && __packet_get_status(po, ph) ==
				   TP_STATUS_AVAILABLE) {
				/* skb was destructed already */
				skb = NULL;
				goto out_status;
			}
			/*
			 * skb was dropped but not destructed yet;
			 * let's treat it like congestion or err < 0
			 */
			err = 0;
		}
		packet_increment_head(&po->tx_ring);
		len_sum += tp_len;
	} while (likely((ph != NULL) ||
		/* Note: packet_read_pending() might be slow if we have
		 * to call it as it's per_cpu variable, but in fast-path
		 * we already short-circuit the loop with the first
		 * condition, and luckily don't have to go that path
		 * anyway.
		 */
		 (need_wait && packet_read_pending(&po->tx_ring))));

	err = len_sum;
	goto out_put;

out_status:
	__packet_set_status(po, ph, status);
	kfree_skb(skb);
out_put:
	dev_put(dev);
out:
	mutex_unlock(&po->pg_vec_lock);
	return err;
}
