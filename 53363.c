void tty_free_file(struct file *file)
{
	struct tty_file_private *priv = file->private_data;

	file->private_data = NULL;
	kfree(priv);
}
