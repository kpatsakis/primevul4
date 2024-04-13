static inline int is_packetized(struct file *file)
{
	return (file->f_flags & O_DIRECT) != 0;
 }
