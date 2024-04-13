static int hid_alloc_buffers(struct usb_device *dev, struct hid_device *hid)
{
	struct usbhid_device *usbhid = hid->driver_data;

	usbhid->inbuf = usb_alloc_coherent(dev, usbhid->bufsize, GFP_KERNEL,
			&usbhid->inbuf_dma);
	usbhid->outbuf = usb_alloc_coherent(dev, usbhid->bufsize, GFP_KERNEL,
			&usbhid->outbuf_dma);
	usbhid->cr = kmalloc(sizeof(*usbhid->cr), GFP_KERNEL);
	usbhid->ctrlbuf = usb_alloc_coherent(dev, usbhid->bufsize, GFP_KERNEL,
			&usbhid->ctrlbuf_dma);
	if (!usbhid->inbuf || !usbhid->outbuf || !usbhid->cr ||
			!usbhid->ctrlbuf)
		return -1;

	return 0;
}
