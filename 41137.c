int __mnt_want_write_file(struct file *file)
{
	struct inode *inode = file_inode(file);

	if (!(file->f_mode & FMODE_WRITE) || special_file(inode->i_mode))
		return __mnt_want_write(file->f_path.mnt);
	else
		return mnt_clone_write(file->f_path.mnt);
}
