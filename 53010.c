static void powermate_free_buffers(struct usb_device *udev, struct powermate_device *pm)
{
	usb_free_coherent(udev, POWERMATE_PAYLOAD_SIZE_MAX,
			  pm->data, pm->data_dma);
	kfree(pm->configcr);
}
