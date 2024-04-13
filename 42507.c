int bat_socket_setup(struct bat_priv *bat_priv)
{
	struct dentry *d;

	if (!bat_priv->debug_dir)
		goto err;

	d = debugfs_create_file(ICMP_SOCKET, S_IFREG | S_IWUSR | S_IRUSR,
				bat_priv->debug_dir, bat_priv, &fops);
	if (d)
		goto err;

	return 0;

err:
	return 1;
}
