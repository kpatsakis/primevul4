static ssize_t ndp_to_end_store(struct device *d,  struct device_attribute *attr, const char *buf, size_t len)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	bool enable;

	if (strtobool(buf, &enable))
		return -EINVAL;

	/* no change? */
	if (enable == (ctx->drvflags & CDC_NCM_FLAG_NDP_TO_END))
		return len;

	if (enable && !ctx->delayed_ndp16) {
		ctx->delayed_ndp16 = kzalloc(ctx->max_ndp_size, GFP_KERNEL);
		if (!ctx->delayed_ndp16)
			return -ENOMEM;
	}

	/* flush pending data before changing flag */
	netif_tx_lock_bh(dev->net);
	usbnet_start_xmit(NULL, dev->net);
	spin_lock_bh(&ctx->mtx);
	if (enable)
		ctx->drvflags |= CDC_NCM_FLAG_NDP_TO_END;
	else
		ctx->drvflags &= ~CDC_NCM_FLAG_NDP_TO_END;
	spin_unlock_bh(&ctx->mtx);
	netif_tx_unlock_bh(dev->net);

	return len;
}
