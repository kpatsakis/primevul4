static int ubifs_file_mmap(struct file *file, struct vm_area_struct *vma)
{
	int err;

	err = generic_file_mmap(file, vma);
	if (err)
		return err;
	vma->vm_ops = &ubifs_file_vm_ops;
	return 0;
}
