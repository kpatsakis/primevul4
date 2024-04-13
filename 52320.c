static int cdc_ncm_eth_hlen(struct usbnet *dev)
{
	if (cdc_ncm_comm_intf_is_mbim(dev->intf->cur_altsetting))
		return 0;
	return ETH_HLEN;
}
