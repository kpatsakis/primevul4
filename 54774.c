static struct urb *simple_alloc_urb(
	struct usb_device	*udev,
	int			pipe,
	unsigned long		bytes,
	u8			bInterval)
{
	return usbtest_alloc_urb(udev, pipe, bytes, URB_NO_TRANSFER_DMA_MAP, 0,
			bInterval, simple_callback);
}
