int security_get_classes(char ***classes, int *nclasses)
{
	int rc;

	read_lock(&policy_rwlock);

	rc = -ENOMEM;
	*nclasses = policydb.p_classes.nprim;
	*classes = kcalloc(*nclasses, sizeof(**classes), GFP_ATOMIC);
	if (!*classes)
		goto out;

	rc = hashtab_map(policydb.p_classes.table, get_classes_callback,
			*classes);
	if (rc) {
		int i;
		for (i = 0; i < *nclasses; i++)
			kfree((*classes)[i]);
		kfree(*classes);
	}

out:
	read_unlock(&policy_rwlock);
	return rc;
}
