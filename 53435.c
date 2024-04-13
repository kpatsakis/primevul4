int ovl_set_attr(struct dentry *upperdentry, struct kstat *stat)
{
	int err = 0;

	if (!S_ISLNK(stat->mode)) {
		struct iattr attr = {
			.ia_valid = ATTR_MODE,
			.ia_mode = stat->mode,
		};
		err = notify_change(upperdentry, &attr, NULL);
	}
	if (!err) {
		struct iattr attr = {
			.ia_valid = ATTR_UID | ATTR_GID,
			.ia_uid = stat->uid,
			.ia_gid = stat->gid,
		};
		err = notify_change(upperdentry, &attr, NULL);
	}
	if (!err)
		ovl_set_timestamps(upperdentry, stat);

	return err;
}
