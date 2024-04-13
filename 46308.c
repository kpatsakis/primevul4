static int shm_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct shm_file_data *sfd = shm_file_data(file);
	int ret;

	ret = sfd->file->f_op->mmap(sfd->file, vma);
	if (ret != 0)
		return ret;
	sfd->vm_ops = vma->vm_ops;
#ifdef CONFIG_MMU
	WARN_ON(!sfd->vm_ops->fault);
#endif
	vma->vm_ops = &shm_vm_ops;
	shm_open(vma);

	return ret;
}
