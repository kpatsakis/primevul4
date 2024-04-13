static void usbdev_vm_close(struct vm_area_struct *vma)
{
	struct usb_memory *usbm = vma->vm_private_data;

	dec_usb_memory_use_count(usbm, &usbm->vma_use_count);
}
