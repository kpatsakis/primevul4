long user_read(const struct key *key, char __user *buffer, size_t buflen)
{
	struct user_key_payload *upayload;
	long ret;

	upayload = rcu_dereference_key(key);
	ret = upayload->datalen;

	/* we can return the data as is */
	if (buffer && buflen > 0) {
		if (buflen > upayload->datalen)
			buflen = upayload->datalen;

		if (copy_to_user(buffer, upayload->data, buflen) != 0)
			ret = -EFAULT;
	}

	return ret;
}
