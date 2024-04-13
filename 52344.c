static ssize_t cdc_ncm_store_tx_max(struct device *d,  struct device_attribute *attr, const char *buf, size_t len)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	unsigned long val;

	if (kstrtoul(buf, 0, &val) || cdc_ncm_check_tx_max(dev, val) != val)
		return -EINVAL;

	cdc_ncm_update_rxtx_max(dev, ctx->rx_max, val);
	return len;
}
