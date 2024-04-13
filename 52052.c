static void usbdev_vm_open(struct vm_area_struct *vma)
{
	struct usb_memory *usbm = vma->vm_private_data;
	unsigned long flags;

	spin_lock_irqsave(&usbm->ps->lock, flags);
	++usbm->vma_use_count;
	spin_unlock_irqrestore(&usbm->ps->lock, flags);
}
