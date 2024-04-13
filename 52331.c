static struct usb_cdc_ncm_ndp16 *cdc_ncm_ndp(struct cdc_ncm_ctx *ctx, struct sk_buff *skb, __le32 sign, size_t reserve)
{
	struct usb_cdc_ncm_ndp16 *ndp16 = NULL;
	struct usb_cdc_ncm_nth16 *nth16 = (void *)skb->data;
	size_t ndpoffset = le16_to_cpu(nth16->wNdpIndex);

	/* If NDP should be moved to the end of the NCM package, we can't follow the
	* NTH16 header as we would normally do. NDP isn't written to the SKB yet, and
	* the wNdpIndex field in the header is actually not consistent with reality. It will be later.
	*/
	if (ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END) {
		if (ctx->delayed_ndp16->dwSignature == sign)
			return ctx->delayed_ndp16;

		/* We can only push a single NDP to the end. Return
		 * NULL to send what we've already got and queue this
		 * skb for later.
		 */
		else if (ctx->delayed_ndp16->dwSignature)
			return NULL;
	}

	/* follow the chain of NDPs, looking for a match */
	while (ndpoffset) {
		ndp16 = (struct usb_cdc_ncm_ndp16 *)(skb->data + ndpoffset);
		if  (ndp16->dwSignature == sign)
			return ndp16;
		ndpoffset = le16_to_cpu(ndp16->wNextNdpIndex);
	}

	/* align new NDP */
	if (!(ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END))
		cdc_ncm_align_tail(skb, ctx->tx_ndp_modulus, 0, ctx->tx_max);

	/* verify that there is room for the NDP and the datagram (reserve) */
	if ((ctx->tx_max - skb->len - reserve) < ctx->max_ndp_size)
		return NULL;

	/* link to it */
	if (ndp16)
		ndp16->wNextNdpIndex = cpu_to_le16(skb->len);
	else
		nth16->wNdpIndex = cpu_to_le16(skb->len);

	/* push a new empty NDP */
	if (!(ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END))
		ndp16 = (struct usb_cdc_ncm_ndp16 *)memset(skb_put(skb, ctx->max_ndp_size), 0, ctx->max_ndp_size);
	else
		ndp16 = ctx->delayed_ndp16;

	ndp16->dwSignature = sign;
	ndp16->wLength = cpu_to_le16(sizeof(struct usb_cdc_ncm_ndp16) + sizeof(struct usb_cdc_ncm_dpe16));
	return ndp16;
}
