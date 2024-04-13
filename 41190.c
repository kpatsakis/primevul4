void mnt_drop_write_file(struct file *file)
{
	mnt_drop_write(file->f_path.mnt);
}
