int tty_add_file(struct tty_struct *tty, struct file *file)
{
	struct tty_file_private *priv;

	priv = kmalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->tty = tty;
	priv->file = file;
	file->private_data = priv;

	spin_lock(&tty_files_lock);
	list_add(&priv->list, &tty->tty_files);
	spin_unlock(&tty_files_lock);

	return 0;
}
