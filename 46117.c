static void ovl_put_link(struct inode *unused, void *c)
{
	struct inode *realinode;
	struct ovl_link_data *data = c;

	if (!data)
		return;

	realinode = data->realdentry->d_inode;
	realinode->i_op->put_link(realinode, data->cookie);
	kfree(data);
}
