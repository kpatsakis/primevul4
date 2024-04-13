static loff_t tower_llseek (struct file *file, loff_t off, int whence)
{
	return -ESPIPE;		/* unseekable */
}
