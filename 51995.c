static void dec_usb_memory_use_count(struct usb_memory *usbm, int *count)
{
	struct usb_dev_state *ps = usbm->ps;
	unsigned long flags;

	spin_lock_irqsave(&ps->lock, flags);
	--*count;
	if (usbm->urb_use_count == 0 && usbm->vma_use_count == 0) {
		list_del(&usbm->memlist);
		spin_unlock_irqrestore(&ps->lock, flags);

		usb_free_coherent(ps->dev, usbm->size, usbm->mem,
				usbm->dma_handle);
		usbfs_decrease_memory_usage(
			usbm->size + sizeof(struct usb_memory));
		kfree(usbm);
	} else {
		spin_unlock_irqrestore(&ps->lock, flags);
	}
}
