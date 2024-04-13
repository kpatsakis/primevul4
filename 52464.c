static void acm_read_buffers_free(struct acm *acm)
{
	struct usb_device *usb_dev = interface_to_usbdev(acm->control);
	int i;

	for (i = 0; i < acm->rx_buflimit; i++)
		usb_free_coherent(usb_dev, acm->readsize,
			  acm->read_buffers[i].base, acm->read_buffers[i].dma);
}
