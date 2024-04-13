static struct key *nfs_idmap_request_key(const char *name, size_t namelen,
					 const char *type, struct idmap *idmap)
{
	char *desc;
	struct key *rkey;
	ssize_t ret;

	ret = nfs_idmap_get_desc(name, namelen, type, strlen(type), &desc);
	if (ret <= 0)
		return ERR_PTR(ret);

	rkey = request_key(&key_type_id_resolver, desc, "");
	if (IS_ERR(rkey)) {
		mutex_lock(&idmap->idmap_mutex);
		rkey = request_key_with_auxdata(&key_type_id_resolver_legacy,
						desc, "", 0, idmap);
		mutex_unlock(&idmap->idmap_mutex);
	}
	if (!IS_ERR(rkey))
		set_bit(KEY_FLAG_ROOT_CAN_INVAL, &rkey->flags);

	kfree(desc);
	return rkey;
}
