int security_get_bool_value(int bool)
{
	int rc;
	int len;

	read_lock(&policy_rwlock);

	rc = -EFAULT;
	len = policydb.p_bools.nprim;
	if (bool >= len)
		goto out;

	rc = policydb.bool_val_to_struct[bool]->state;
out:
	read_unlock(&policy_rwlock);
	return rc;
}
