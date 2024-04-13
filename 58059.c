static void catc_rx_done(struct urb *urb)
{
	struct catc *catc = urb->context;
	u8 *pkt_start = urb->transfer_buffer;
	struct sk_buff *skb;
	int pkt_len, pkt_offset = 0;
	int status = urb->status;

	if (!catc->is_f5u011) {
		clear_bit(RX_RUNNING, &catc->flags);
		pkt_offset = 2;
	}

	if (status) {
		dev_dbg(&urb->dev->dev, "rx_done, status %d, length %d\n",
			status, urb->actual_length);
		return;
	}

	do {
		if(!catc->is_f5u011) {
			pkt_len = le16_to_cpup((__le16*)pkt_start);
			if (pkt_len > urb->actual_length) {
				catc->netdev->stats.rx_length_errors++;
				catc->netdev->stats.rx_errors++;
				break;
			}
		} else {
			pkt_len = urb->actual_length;
		}

		if (!(skb = dev_alloc_skb(pkt_len)))
			return;

		skb_copy_to_linear_data(skb, pkt_start + pkt_offset, pkt_len);
		skb_put(skb, pkt_len);

		skb->protocol = eth_type_trans(skb, catc->netdev);
		netif_rx(skb);

		catc->netdev->stats.rx_packets++;
		catc->netdev->stats.rx_bytes += pkt_len;

		/* F5U011 only does one packet per RX */
		if (catc->is_f5u011)
			break;
		pkt_start += (((pkt_len + 1) >> 6) + 1) << 6;

	} while (pkt_start - (u8 *) urb->transfer_buffer < urb->actual_length);

	if (catc->is_f5u011) {
		if (atomic_read(&catc->recq_sz)) {
			int state;
			atomic_dec(&catc->recq_sz);
			netdev_dbg(catc->netdev, "getting extra packet\n");
			urb->dev = catc->usbdev;
			if ((state = usb_submit_urb(urb, GFP_ATOMIC)) < 0) {
				netdev_dbg(catc->netdev,
					   "submit(rx_urb) status %d\n", state);
			}
		} else {
			clear_bit(RX_RUNNING, &catc->flags);
		}
	}
}
