cdc_ncm_speed_change(struct usbnet *dev,
		     struct usb_cdc_speed_change *data)
{
	uint32_t rx_speed = le32_to_cpu(data->DLBitRRate);
	uint32_t tx_speed = le32_to_cpu(data->ULBitRate);

	/*
	 * Currently the USB-NET API does not support reporting the actual
	 * device speed. Do print it instead.
	 */
	if ((tx_speed > 1000000) && (rx_speed > 1000000)) {
		netif_info(dev, link, dev->net,
			   "%u mbit/s downlink %u mbit/s uplink\n",
			   (unsigned int)(rx_speed / 1000000U),
			   (unsigned int)(tx_speed / 1000000U));
	} else {
		netif_info(dev, link, dev->net,
			   "%u kbit/s downlink %u kbit/s uplink\n",
			   (unsigned int)(rx_speed / 1000U),
			   (unsigned int)(tx_speed / 1000U));
	}
}
