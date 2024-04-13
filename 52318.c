static u32 cdc_ncm_check_rx_max(struct usbnet *dev, u32 new_rx)
{
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	u32 val, max, min;

	/* clamp new_rx to sane values */
	min = USB_CDC_NCM_NTB_MIN_IN_SIZE;
	max = min_t(u32, CDC_NCM_NTB_MAX_SIZE_RX, le32_to_cpu(ctx->ncm_parm.dwNtbInMaxSize));

	/* dwNtbInMaxSize spec violation? Use MIN size for both limits */
	if (max < min) {
		dev_warn(&dev->intf->dev, "dwNtbInMaxSize=%u is too small. Using %u\n",
			 le32_to_cpu(ctx->ncm_parm.dwNtbInMaxSize), min);
		max = min;
	}

	val = clamp_t(u32, new_rx, min, max);
	if (val != new_rx)
		dev_dbg(&dev->intf->dev, "rx_max must be in the [%u, %u] range\n", min, max);

	return val;
}
