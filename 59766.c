static ssize_t nfs_idmap_get_key(const char *name, size_t namelen,
				 const char *type, void *data,
				 size_t data_size, struct idmap *idmap)
{
	const struct cred *saved_cred;
	struct key *rkey;
	struct user_key_payload *payload;
	ssize_t ret;

	saved_cred = override_creds(id_resolver_cache);
	rkey = nfs_idmap_request_key(name, namelen, type, idmap);
	revert_creds(saved_cred);

	if (IS_ERR(rkey)) {
		ret = PTR_ERR(rkey);
		goto out;
	}

	rcu_read_lock();
	rkey->perm |= KEY_USR_VIEW;

	ret = key_validate(rkey);
	if (ret < 0)
		goto out_up;

	payload = rcu_dereference(rkey->payload.rcudata);
	if (IS_ERR_OR_NULL(payload)) {
		ret = PTR_ERR(payload);
		goto out_up;
	}

	ret = payload->datalen;
	if (ret > 0 && ret <= data_size)
		memcpy(data, payload->data, ret);
	else
		ret = -EINVAL;

out_up:
	rcu_read_unlock();
	key_put(rkey);
out:
	return ret;
}
