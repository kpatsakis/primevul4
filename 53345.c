void tty_add_file(struct tty_struct *tty, struct file *file)
{
	struct tty_file_private *priv = file->private_data;

	priv->tty = tty;
	priv->file = file;

	spin_lock(&tty_files_lock);
	list_add(&priv->list, &tty->tty_files);
	spin_unlock(&tty_files_lock);
}
