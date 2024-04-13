static u8 cdc_ncm_flags(struct usbnet *dev)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];

	if (cdc_ncm_comm_intf_is_mbim(dev->intf->cur_altsetting) && ctx->mbim_desc)
		return ctx->mbim_desc->bmNetworkCapabilities;
	if (ctx->func_desc)
		return ctx->func_desc->bmNetworkCapabilities;
	return 0;
}
