int security_get_permissions(char *class, char ***perms, int *nperms)
{
	int rc, i;
	struct class_datum *match;

	read_lock(&policy_rwlock);

	rc = -EINVAL;
	match = hashtab_search(policydb.p_classes.table, class);
	if (!match) {
		printk(KERN_ERR "SELinux: %s:  unrecognized class %s\n",
			__func__, class);
		goto out;
	}

	rc = -ENOMEM;
	*nperms = match->permissions.nprim;
	*perms = kcalloc(*nperms, sizeof(**perms), GFP_ATOMIC);
	if (!*perms)
		goto out;

	if (match->comdatum) {
		rc = hashtab_map(match->comdatum->permissions.table,
				get_permissions_callback, *perms);
		if (rc)
			goto err;
	}

	rc = hashtab_map(match->permissions.table, get_permissions_callback,
			*perms);
	if (rc)
		goto err;

out:
	read_unlock(&policy_rwlock);
	return rc;

err:
	read_unlock(&policy_rwlock);
	for (i = 0; i < *nperms; i++)
		kfree((*perms)[i]);
	kfree(*perms);
	return rc;
}
