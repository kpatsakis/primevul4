long big_key_read(const struct key *key, char __user *buffer, size_t buflen)
{
	unsigned long datalen = key->type_data.x[1];
	long ret;

	if (!buffer || buflen < datalen)
		return datalen;

	if (datalen > BIG_KEY_FILE_THRESHOLD) {
		struct path *path = (struct path *)&key->payload.data2;
		struct file *file;
		loff_t pos;

		file = dentry_open(path, O_RDONLY, current_cred());
		if (IS_ERR(file))
			return PTR_ERR(file);

		pos = 0;
		ret = vfs_read(file, buffer, datalen, &pos);
		fput(file);
		if (ret >= 0 && ret != datalen)
			ret = -EIO;
	} else {
		ret = datalen;
		if (copy_to_user(buffer, key->payload.data, datalen) != 0)
			ret = -EFAULT;
	}

	return ret;
}
