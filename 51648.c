static int vop_mmap(struct file *f, struct vm_area_struct *vma)
{
	struct vop_vdev *vdev = f->private_data;
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	unsigned long pa, size = vma->vm_end - vma->vm_start, size_rem = size;
	int i, err;

	err = vop_vdev_inited(vdev);
	if (err)
		goto ret;
	if (vma->vm_flags & VM_WRITE) {
		err = -EACCES;
		goto ret;
	}
	while (size_rem) {
		i = vop_query_offset(vdev, offset, &size, &pa);
		if (i < 0) {
			err = -EINVAL;
			goto ret;
		}
		err = remap_pfn_range(vma, vma->vm_start + offset,
				      pa >> PAGE_SHIFT, size,
				      vma->vm_page_prot);
		if (err)
			goto ret;
		size_rem -= size;
		offset += size;
	}
ret:
	return err;
}
