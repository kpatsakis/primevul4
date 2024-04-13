static int powermate_alloc_buffers(struct usb_device *udev, struct powermate_device *pm)
{
	pm->data = usb_alloc_coherent(udev, POWERMATE_PAYLOAD_SIZE_MAX,
				      GFP_ATOMIC, &pm->data_dma);
	if (!pm->data)
		return -1;

	pm->configcr = kmalloc(sizeof(*(pm->configcr)), GFP_KERNEL);
	if (!pm->configcr)
		return -ENOMEM;

	return 0;
}
