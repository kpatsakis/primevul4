static ssize_t tun_get_user(struct tun_struct *tun, void *msg_control,
			    const struct iovec *iv, size_t total_len,
			    size_t count, int noblock)
{
	struct tun_pi pi = { 0, cpu_to_be16(ETH_P_IP) };
	struct sk_buff *skb;
	size_t len = total_len, align = NET_SKB_PAD;
	struct virtio_net_hdr gso = { 0 };
	int offset = 0;
	int copylen;
	bool zerocopy = false;
	int err;

	if (!(tun->flags & TUN_NO_PI)) {
		if ((len -= sizeof(pi)) > total_len)
			return -EINVAL;

		if (memcpy_fromiovecend((void *)&pi, iv, 0, sizeof(pi)))
			return -EFAULT;
		offset += sizeof(pi);
	}

	if (tun->flags & TUN_VNET_HDR) {
		if ((len -= tun->vnet_hdr_sz) > total_len)
			return -EINVAL;

		if (memcpy_fromiovecend((void *)&gso, iv, offset, sizeof(gso)))
			return -EFAULT;

		if ((gso.flags & VIRTIO_NET_HDR_F_NEEDS_CSUM) &&
		    gso.csum_start + gso.csum_offset + 2 > gso.hdr_len)
			gso.hdr_len = gso.csum_start + gso.csum_offset + 2;

		if (gso.hdr_len > len)
			return -EINVAL;
		offset += tun->vnet_hdr_sz;
	}

	if ((tun->flags & TUN_TYPE_MASK) == TUN_TAP_DEV) {
		align += NET_IP_ALIGN;
		if (unlikely(len < ETH_HLEN ||
			     (gso.hdr_len && gso.hdr_len < ETH_HLEN)))
			return -EINVAL;
	}

	if (msg_control)
		zerocopy = true;

	if (zerocopy) {
		/* Userspace may produce vectors with count greater than
		 * MAX_SKB_FRAGS, so we need to linearize parts of the skb
		 * to let the rest of data to be fit in the frags.
		 */
		if (count > MAX_SKB_FRAGS) {
			copylen = iov_length(iv, count - MAX_SKB_FRAGS);
			if (copylen < offset)
				copylen = 0;
			else
				copylen -= offset;
		} else
				copylen = 0;
		/* There are 256 bytes to be copied in skb, so there is enough
		 * room for skb expand head in case it is used.
		 * The rest of the buffer is mapped from userspace.
		 */
		if (copylen < gso.hdr_len)
			copylen = gso.hdr_len;
		if (!copylen)
			copylen = GOODCOPY_LEN;
	} else
		copylen = len;

	skb = tun_alloc_skb(tun, align, copylen, gso.hdr_len, noblock);
	if (IS_ERR(skb)) {
		if (PTR_ERR(skb) != -EAGAIN)
			tun->dev->stats.rx_dropped++;
		return PTR_ERR(skb);
	}

	if (zerocopy)
		err = zerocopy_sg_from_iovec(skb, iv, offset, count);
	else
		err = skb_copy_datagram_from_iovec(skb, 0, iv, offset, len);

	if (err) {
		tun->dev->stats.rx_dropped++;
		kfree_skb(skb);
		return -EFAULT;
	}

	if (gso.flags & VIRTIO_NET_HDR_F_NEEDS_CSUM) {
		if (!skb_partial_csum_set(skb, gso.csum_start,
					  gso.csum_offset)) {
			tun->dev->stats.rx_frame_errors++;
			kfree_skb(skb);
			return -EINVAL;
		}
	}

	switch (tun->flags & TUN_TYPE_MASK) {
	case TUN_TUN_DEV:
		if (tun->flags & TUN_NO_PI) {
			switch (skb->data[0] & 0xf0) {
			case 0x40:
				pi.proto = htons(ETH_P_IP);
				break;
			case 0x60:
				pi.proto = htons(ETH_P_IPV6);
				break;
			default:
				tun->dev->stats.rx_dropped++;
				kfree_skb(skb);
				return -EINVAL;
			}
		}

		skb_reset_mac_header(skb);
		skb->protocol = pi.proto;
		skb->dev = tun->dev;
		break;
	case TUN_TAP_DEV:
		skb->protocol = eth_type_trans(skb, tun->dev);
		break;
	}

	if (gso.gso_type != VIRTIO_NET_HDR_GSO_NONE) {
		pr_debug("GSO!\n");
		switch (gso.gso_type & ~VIRTIO_NET_HDR_GSO_ECN) {
		case VIRTIO_NET_HDR_GSO_TCPV4:
			skb_shinfo(skb)->gso_type = SKB_GSO_TCPV4;
			break;
		case VIRTIO_NET_HDR_GSO_TCPV6:
			skb_shinfo(skb)->gso_type = SKB_GSO_TCPV6;
			break;
		case VIRTIO_NET_HDR_GSO_UDP:
			skb_shinfo(skb)->gso_type = SKB_GSO_UDP;
			break;
		default:
			tun->dev->stats.rx_frame_errors++;
			kfree_skb(skb);
			return -EINVAL;
		}

		if (gso.gso_type & VIRTIO_NET_HDR_GSO_ECN)
			skb_shinfo(skb)->gso_type |= SKB_GSO_TCP_ECN;

		skb_shinfo(skb)->gso_size = gso.gso_size;
		if (skb_shinfo(skb)->gso_size == 0) {
			tun->dev->stats.rx_frame_errors++;
			kfree_skb(skb);
			return -EINVAL;
		}

		/* Header must be checked, and gso_segs computed. */
		skb_shinfo(skb)->gso_type |= SKB_GSO_DODGY;
		skb_shinfo(skb)->gso_segs = 0;
	}

	/* copy skb_ubuf_info for callback when skb has no error */
	if (zerocopy) {
		skb_shinfo(skb)->destructor_arg = msg_control;
		skb_shinfo(skb)->tx_flags |= SKBTX_DEV_ZEROCOPY;
	}

	netif_rx_ni(skb);

	tun->dev->stats.rx_packets++;
	tun->dev->stats.rx_bytes += len;

	return total_len;
}
