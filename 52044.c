static int usbdev_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct usb_memory *usbm = NULL;
	struct usb_dev_state *ps = file->private_data;
	size_t size = vma->vm_end - vma->vm_start;
	void *mem;
	unsigned long flags;
	dma_addr_t dma_handle;
	int ret;

	ret = usbfs_increase_memory_usage(size + sizeof(struct usb_memory));
	if (ret)
		goto error;

	usbm = kzalloc(sizeof(struct usb_memory), GFP_KERNEL);
	if (!usbm) {
		ret = -ENOMEM;
		goto error_decrease_mem;
	}

	mem = usb_alloc_coherent(ps->dev, size, GFP_USER, &dma_handle);
	if (!mem) {
		ret = -ENOMEM;
		goto error_free_usbm;
	}

	memset(mem, 0, size);

	usbm->mem = mem;
	usbm->dma_handle = dma_handle;
	usbm->size = size;
	usbm->ps = ps;
	usbm->vm_start = vma->vm_start;
	usbm->vma_use_count = 1;
	INIT_LIST_HEAD(&usbm->memlist);

	if (remap_pfn_range(vma, vma->vm_start,
			virt_to_phys(usbm->mem) >> PAGE_SHIFT,
			size, vma->vm_page_prot) < 0) {
		dec_usb_memory_use_count(usbm, &usbm->vma_use_count);
		return -EAGAIN;
	}

	vma->vm_flags |= VM_IO;
	vma->vm_flags |= (VM_DONTEXPAND | VM_DONTDUMP);
	vma->vm_ops = &usbdev_vm_ops;
	vma->vm_private_data = usbm;

	spin_lock_irqsave(&ps->lock, flags);
	list_add_tail(&usbm->memlist, &ps->memory_list);
	spin_unlock_irqrestore(&ps->lock, flags);

	return 0;

error_free_usbm:
	kfree(usbm);
error_decrease_mem:
	usbfs_decrease_memory_usage(size + sizeof(struct usb_memory));
error:
	return ret;
}
