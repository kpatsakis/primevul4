int security_policycap_supported(unsigned int req_cap)
{
	int rc;

	read_lock(&policy_rwlock);
	rc = ebitmap_get_bit(&policydb.policycaps, req_cap);
	read_unlock(&policy_rwlock);

	return rc;
}
