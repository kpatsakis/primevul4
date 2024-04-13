static ssize_t nfs_idmap_lookup_name(__u32 id, const char *type, char *buf,
				     size_t buflen, struct idmap *idmap)
{
	char id_str[NFS_UINT_MAXLEN];
	int id_len;
	ssize_t ret;

	id_len = snprintf(id_str, sizeof(id_str), "%u", id);
	ret = nfs_idmap_get_key(id_str, id_len, type, buf, buflen, idmap);
	if (ret < 0)
		return -EINVAL;
	return ret;
}
