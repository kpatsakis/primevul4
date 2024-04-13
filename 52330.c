static u32 cdc_ncm_min_dgram_size(struct usbnet *dev)
{
	if (cdc_ncm_comm_intf_is_mbim(dev->intf->cur_altsetting))
		return CDC_MBIM_MIN_DATAGRAM_SIZE;
	return CDC_NCM_MIN_DATAGRAM_SIZE;
}
