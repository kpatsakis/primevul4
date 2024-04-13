static int comedi_mmap(struct file *file, struct vm_area_struct *vma)
{
	const unsigned minor = iminor(file->f_dentry->d_inode);
	struct comedi_device_file_info *dev_file_info =
	    comedi_get_device_file_info(minor);
	struct comedi_device *dev = dev_file_info->device;
	struct comedi_async *async = NULL;
	unsigned long start = vma->vm_start;
	unsigned long size;
	int n_pages;
	int i;
	int retval;
	struct comedi_subdevice *s;

	mutex_lock(&dev->mutex);
	if (!dev->attached) {
		DPRINTK("no driver configured on comedi%i\n", dev->minor);
		retval = -ENODEV;
		goto done;
	}
	if (vma->vm_flags & VM_WRITE)
		s = comedi_get_write_subdevice(dev_file_info);
	else
		s = comedi_get_read_subdevice(dev_file_info);

	if (s == NULL) {
		retval = -EINVAL;
		goto done;
	}
	async = s->async;
	if (async == NULL) {
		retval = -EINVAL;
		goto done;
	}

	if (vma->vm_pgoff != 0) {
		DPRINTK("comedi: mmap() offset must be 0.\n");
		retval = -EINVAL;
		goto done;
	}

	size = vma->vm_end - vma->vm_start;
	if (size > async->prealloc_bufsz) {
		retval = -EFAULT;
		goto done;
	}
	if (size & (~PAGE_MASK)) {
		retval = -EFAULT;
		goto done;
	}

	n_pages = size >> PAGE_SHIFT;
	for (i = 0; i < n_pages; ++i) {
		if (remap_pfn_range(vma, start,
				    page_to_pfn(virt_to_page
						(async->buf_page_list
						 [i].virt_addr)), PAGE_SIZE,
				    PAGE_SHARED)) {
			retval = -EAGAIN;
			goto done;
		}
		start += PAGE_SIZE;
	}

	vma->vm_ops = &comedi_vm_ops;
	vma->vm_private_data = async;

	async->mmap_count++;

	retval = 0;
done:
	mutex_unlock(&dev->mutex);
	return retval;
}
