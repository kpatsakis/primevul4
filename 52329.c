static u32 cdc_ncm_max_dgram_size(struct usbnet *dev)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];

	if (cdc_ncm_comm_intf_is_mbim(dev->intf->cur_altsetting) && ctx->mbim_desc)
		return le16_to_cpu(ctx->mbim_desc->wMaxSegmentSize);
	if (ctx->ether_desc)
		return le16_to_cpu(ctx->ether_desc->wMaxSegmentSize);
	return CDC_NCM_MAX_DATAGRAM_SIZE;
}
