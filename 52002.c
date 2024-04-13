find_memory_area(struct usb_dev_state *ps, const struct usbdevfs_urb *uurb)
{
	struct usb_memory *usbm = NULL, *iter;
	unsigned long flags;
	unsigned long uurb_start = (unsigned long)uurb->buffer;

	spin_lock_irqsave(&ps->lock, flags);
	list_for_each_entry(iter, &ps->memory_list, memlist) {
		if (uurb_start >= iter->vm_start &&
				uurb_start < iter->vm_start + iter->size) {
			if (uurb->buffer_length > iter->vm_start + iter->size -
					uurb_start) {
				usbm = ERR_PTR(-EINVAL);
			} else {
				usbm = iter;
				usbm->urb_use_count++;
			}
			break;
		}
	}
	spin_unlock_irqrestore(&ps->lock, flags);
	return usbm;
}
