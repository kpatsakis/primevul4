static int f2fs_file_mmap(struct file *file, struct vm_area_struct *vma)
{
	file_accessed(file);
	vma->vm_ops = &f2fs_file_vm_ops;
	return 0;
}
