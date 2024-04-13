struct pipe_inode_info *get_pipe_info(struct file *file)
{
	return file->f_op == &pipefifo_fops ? file->private_data : NULL;
}
