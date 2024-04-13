static int shmem_set_policy(struct vm_area_struct *vma, struct mempolicy *mpol)
{
	struct inode *inode = vma->vm_file->f_path.dentry->d_inode;
	return mpol_set_shared_policy(&SHMEM_I(inode)->policy, vma, mpol);
}
