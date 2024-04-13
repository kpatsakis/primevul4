static long dns_resolver_read(const struct key *key,
			      char __user *buffer, size_t buflen)
{
	if (key->type_data.x[0])
		return key->type_data.x[0];

	return user_read(key, buffer, buflen);
}
