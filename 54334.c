void tty_del_file(struct file *file)
{
	struct tty_file_private *priv = file->private_data;

	spin_lock(&tty_files_lock);
	list_del(&priv->list);
	spin_unlock(&tty_files_lock);
	file->private_data = NULL;
	kfree(priv);
}
