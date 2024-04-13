static enum d_walk_ret detach_and_collect(void *_data, struct dentry *dentry)
{
	struct detach_data *data = _data;

	if (d_mountpoint(dentry)) {
		__dget_dlock(dentry);
		data->mountpoint = dentry;
		return D_WALK_QUIT;
	}

	return select_collect(&data->select, dentry);
}
