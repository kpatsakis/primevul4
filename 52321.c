cdc_ncm_fill_tx_frame(struct usbnet *dev, struct sk_buff *skb, __le32 sign)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	struct usb_cdc_ncm_nth16 *nth16;
	struct usb_cdc_ncm_ndp16 *ndp16;
	struct sk_buff *skb_out;
	u16 n = 0, index, ndplen;
	u8 ready2send = 0;
	u32 delayed_ndp_size;

	/* When our NDP gets written in cdc_ncm_ndp(), then skb_out->len gets updated
	 * accordingly. Otherwise, we should check here.
	 */
	if (ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END)
		delayed_ndp_size = ctx->max_ndp_size;
	else
		delayed_ndp_size = 0;

	/* if there is a remaining skb, it gets priority */
	if (skb != NULL) {
		swap(skb, ctx->tx_rem_skb);
		swap(sign, ctx->tx_rem_sign);
	} else {
		ready2send = 1;
	}

	/* check if we are resuming an OUT skb */
	skb_out = ctx->tx_curr_skb;

	/* allocate a new OUT skb */
	if (!skb_out) {
		skb_out = alloc_skb(ctx->tx_max, GFP_ATOMIC);
		if (skb_out == NULL) {
			if (skb != NULL) {
				dev_kfree_skb_any(skb);
				dev->net->stats.tx_dropped++;
			}
			goto exit_no_skb;
		}
		/* fill out the initial 16-bit NTB header */
		nth16 = (struct usb_cdc_ncm_nth16 *)memset(skb_put(skb_out, sizeof(struct usb_cdc_ncm_nth16)), 0, sizeof(struct usb_cdc_ncm_nth16));
		nth16->dwSignature = cpu_to_le32(USB_CDC_NCM_NTH16_SIGN);
		nth16->wHeaderLength = cpu_to_le16(sizeof(struct usb_cdc_ncm_nth16));
		nth16->wSequence = cpu_to_le16(ctx->tx_seq++);

		/* count total number of frames in this NTB */
		ctx->tx_curr_frame_num = 0;

		/* recent payload counter for this skb_out */
		ctx->tx_curr_frame_payload = 0;
	}

	for (n = ctx->tx_curr_frame_num; n < ctx->tx_max_datagrams; n++) {
		/* send any remaining skb first */
		if (skb == NULL) {
			skb = ctx->tx_rem_skb;
			sign = ctx->tx_rem_sign;
			ctx->tx_rem_skb = NULL;

			/* check for end of skb */
			if (skb == NULL)
				break;
		}

		/* get the appropriate NDP for this skb */
		ndp16 = cdc_ncm_ndp(ctx, skb_out, sign, skb->len + ctx->tx_modulus + ctx->tx_remainder);

		/* align beginning of next frame */
		cdc_ncm_align_tail(skb_out,  ctx->tx_modulus, ctx->tx_remainder, ctx->tx_max);

		/* check if we had enough room left for both NDP and frame */
		if (!ndp16 || skb_out->len + skb->len + delayed_ndp_size > ctx->tx_max) {
			if (n == 0) {
				/* won't fit, MTU problem? */
				dev_kfree_skb_any(skb);
				skb = NULL;
				dev->net->stats.tx_dropped++;
			} else {
				/* no room for skb - store for later */
				if (ctx->tx_rem_skb != NULL) {
					dev_kfree_skb_any(ctx->tx_rem_skb);
					dev->net->stats.tx_dropped++;
				}
				ctx->tx_rem_skb = skb;
				ctx->tx_rem_sign = sign;
				skb = NULL;
				ready2send = 1;
				ctx->tx_reason_ntb_full++;	/* count reason for transmitting */
			}
			break;
		}

		/* calculate frame number withing this NDP */
		ndplen = le16_to_cpu(ndp16->wLength);
		index = (ndplen - sizeof(struct usb_cdc_ncm_ndp16)) / sizeof(struct usb_cdc_ncm_dpe16) - 1;

		/* OK, add this skb */
		ndp16->dpe16[index].wDatagramLength = cpu_to_le16(skb->len);
		ndp16->dpe16[index].wDatagramIndex = cpu_to_le16(skb_out->len);
		ndp16->wLength = cpu_to_le16(ndplen + sizeof(struct usb_cdc_ncm_dpe16));
		memcpy(skb_put(skb_out, skb->len), skb->data, skb->len);
		ctx->tx_curr_frame_payload += skb->len;	/* count real tx payload data */
		dev_kfree_skb_any(skb);
		skb = NULL;

		/* send now if this NDP is full */
		if (index >= CDC_NCM_DPT_DATAGRAMS_MAX) {
			ready2send = 1;
			ctx->tx_reason_ndp_full++;	/* count reason for transmitting */
			break;
		}
	}

	/* free up any dangling skb */
	if (skb != NULL) {
		dev_kfree_skb_any(skb);
		skb = NULL;
		dev->net->stats.tx_dropped++;
	}

	ctx->tx_curr_frame_num = n;

	if (n == 0) {
		/* wait for more frames */
		/* push variables */
		ctx->tx_curr_skb = skb_out;
		goto exit_no_skb;

	} else if ((n < ctx->tx_max_datagrams) && (ready2send == 0) && (ctx->timer_interval > 0)) {
		/* wait for more frames */
		/* push variables */
		ctx->tx_curr_skb = skb_out;
		/* set the pending count */
		if (n < CDC_NCM_RESTART_TIMER_DATAGRAM_CNT)
			ctx->tx_timer_pending = CDC_NCM_TIMER_PENDING_CNT;
		goto exit_no_skb;

	} else {
		if (n == ctx->tx_max_datagrams)
			ctx->tx_reason_max_datagram++;	/* count reason for transmitting */
		/* frame goes out */
		/* variables will be reset at next call */
	}

	/* If requested, put NDP at end of frame. */
	if (ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END) {
		nth16 = (struct usb_cdc_ncm_nth16 *)skb_out->data;
		cdc_ncm_align_tail(skb_out, ctx->tx_ndp_modulus, 0, ctx->tx_max);
		nth16->wNdpIndex = cpu_to_le16(skb_out->len);
		memcpy(skb_put(skb_out, ctx->max_ndp_size), ctx->delayed_ndp16, ctx->max_ndp_size);

		/* Zero out delayed NDP - signature checking will naturally fail. */
		ndp16 = memset(ctx->delayed_ndp16, 0, ctx->max_ndp_size);
	}

	/* If collected data size is less or equal ctx->min_tx_pkt
	 * bytes, we send buffers as it is. If we get more data, it
	 * would be more efficient for USB HS mobile device with DMA
	 * engine to receive a full size NTB, than canceling DMA
	 * transfer and receiving a short packet.
	 *
	 * This optimization support is pointless if we end up sending
	 * a ZLP after full sized NTBs.
	 */
	if (!(dev->driver_info->flags & FLAG_SEND_ZLP) &&
	    skb_out->len > ctx->min_tx_pkt)
		memset(skb_put(skb_out, ctx->tx_max - skb_out->len), 0,
		       ctx->tx_max - skb_out->len);
	else if (skb_out->len < ctx->tx_max && (skb_out->len % dev->maxpacket) == 0)
		*skb_put(skb_out, 1) = 0;	/* force short packet */

	/* set final frame length */
	nth16 = (struct usb_cdc_ncm_nth16 *)skb_out->data;
	nth16->wBlockLength = cpu_to_le16(skb_out->len);

	/* return skb */
	ctx->tx_curr_skb = NULL;

	/* keep private stats: framing overhead and number of NTBs */
	ctx->tx_overhead += skb_out->len - ctx->tx_curr_frame_payload;
	ctx->tx_ntbs++;

	/* usbnet will count all the framing overhead by default.
	 * Adjust the stats so that the tx_bytes counter show real
	 * payload data instead.
	 */
	usbnet_set_skb_tx_stats(skb_out, n,
				(long)ctx->tx_curr_frame_payload - skb_out->len);

	return skb_out;

exit_no_skb:
	/* Start timer, if there is a remaining non-empty skb */
	if (ctx->tx_curr_skb != NULL && n > 0)
		cdc_ncm_tx_timeout_start(ctx);
	return NULL;
}
