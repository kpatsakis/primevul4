static ssize_t cdc_ncm_store_tx_timer_usecs(struct device *d,  struct device_attribute *attr, const char *buf, size_t len)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	ssize_t ret;
	unsigned long val;

	ret = kstrtoul(buf, 0, &val);
	if (ret)
		return ret;
	if (val && (val < CDC_NCM_TIMER_INTERVAL_MIN || val > CDC_NCM_TIMER_INTERVAL_MAX))
		return -EINVAL;

	spin_lock_bh(&ctx->mtx);
	ctx->timer_interval = val * NSEC_PER_USEC;
	if (!ctx->timer_interval)
		ctx->tx_timer_pending = 0;
	spin_unlock_bh(&ctx->mtx);
	return len;
}
