size_t security_policydb_len(void)
{
	size_t len;

	read_lock(&policy_rwlock);
	len = policydb.len;
	read_unlock(&policy_rwlock);

	return len;
}
