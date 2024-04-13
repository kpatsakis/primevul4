static u32 cdc_ncm_check_tx_max(struct usbnet *dev, u32 new_tx)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	u32 val, max, min;

	/* clamp new_tx to sane values */
	min = ctx->max_datagram_size + ctx->max_ndp_size + sizeof(struct usb_cdc_ncm_nth16);
	max = min_t(u32, CDC_NCM_NTB_MAX_SIZE_TX, le32_to_cpu(ctx->ncm_parm.dwNtbOutMaxSize));

	/* some devices set dwNtbOutMaxSize too low for the above default */
	min = min(min, max);

	val = clamp_t(u32, new_tx, min, max);
	if (val != new_tx)
		dev_dbg(&dev->intf->dev, "tx_max must be in the [%u, %u] range\n", min, max);

	return val;
}
