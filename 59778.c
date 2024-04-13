static int nfs_idmap_read_and_verify_message(struct idmap_msg *im,
		struct idmap_msg *upcall,
		struct key *key, struct key *authkey)
{
	char id_str[NFS_UINT_MAXLEN];
	size_t len;
	int ret = -ENOKEY;

	/* ret = -ENOKEY */
	if (upcall->im_type != im->im_type || upcall->im_conv != im->im_conv)
		goto out;
	switch (im->im_conv) {
	case IDMAP_CONV_NAMETOID:
		if (strcmp(upcall->im_name, im->im_name) != 0)
			break;
		/* Note: here we store the NUL terminator too */
		len = sprintf(id_str, "%d", im->im_id) + 1;
		ret = nfs_idmap_instantiate(key, authkey, id_str, len);
		break;
	case IDMAP_CONV_IDTONAME:
		if (upcall->im_id != im->im_id)
			break;
		len = strlen(im->im_name);
		ret = nfs_idmap_instantiate(key, authkey, im->im_name, len);
		break;
	default:
		ret = -EINVAL;
	}
out:
	return ret;
}
