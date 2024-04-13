static ssize_t cdc_ncm_store_min_tx_pkt(struct device *d,  struct device_attribute *attr, const char *buf, size_t len)
{
	struct usbnet *dev = netdev_priv(to_net_dev(d));
	struct cdc_ncm_ctx *ctx = (struct cdc_ncm_ctx *)dev->data[0];
	unsigned long val;

	/* no need to restrict values - anything from 0 to infinity is OK */
	if (kstrtoul(buf, 0, &val))
		return -EINVAL;

	ctx->min_tx_pkt = val;
	return len;
}
