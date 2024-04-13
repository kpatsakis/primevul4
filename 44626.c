int security_get_user_sids(u32 fromsid,
			   char *username,
			   u32 **sids,
			   u32 *nel)
{
	struct context *fromcon, usercon;
	u32 *mysids = NULL, *mysids2, sid;
	u32 mynel = 0, maxnel = SIDS_NEL;
	struct user_datum *user;
	struct role_datum *role;
	struct ebitmap_node *rnode, *tnode;
	int rc = 0, i, j;

	*sids = NULL;
	*nel = 0;

	if (!ss_initialized)
		goto out;

	read_lock(&policy_rwlock);

	context_init(&usercon);

	rc = -EINVAL;
	fromcon = sidtab_search(&sidtab, fromsid);
	if (!fromcon)
		goto out_unlock;

	rc = -EINVAL;
	user = hashtab_search(policydb.p_users.table, username);
	if (!user)
		goto out_unlock;

	usercon.user = user->value;

	rc = -ENOMEM;
	mysids = kcalloc(maxnel, sizeof(*mysids), GFP_ATOMIC);
	if (!mysids)
		goto out_unlock;

	ebitmap_for_each_positive_bit(&user->roles, rnode, i) {
		role = policydb.role_val_to_struct[i];
		usercon.role = i + 1;
		ebitmap_for_each_positive_bit(&role->types, tnode, j) {
			usercon.type = j + 1;

			if (mls_setup_user_range(fromcon, user, &usercon))
				continue;

			rc = sidtab_context_to_sid(&sidtab, &usercon, &sid);
			if (rc)
				goto out_unlock;
			if (mynel < maxnel) {
				mysids[mynel++] = sid;
			} else {
				rc = -ENOMEM;
				maxnel += SIDS_NEL;
				mysids2 = kcalloc(maxnel, sizeof(*mysids2), GFP_ATOMIC);
				if (!mysids2)
					goto out_unlock;
				memcpy(mysids2, mysids, mynel * sizeof(*mysids2));
				kfree(mysids);
				mysids = mysids2;
				mysids[mynel++] = sid;
			}
		}
	}
	rc = 0;
out_unlock:
	read_unlock(&policy_rwlock);
	if (rc || !mynel) {
		kfree(mysids);
		goto out;
	}

	rc = -ENOMEM;
	mysids2 = kcalloc(mynel, sizeof(*mysids2), GFP_KERNEL);
	if (!mysids2) {
		kfree(mysids);
		goto out;
	}
	for (i = 0, j = 0; i < mynel; i++) {
		struct av_decision dummy_avd;
		rc = avc_has_perm_noaudit(fromsid, mysids[i],
					  SECCLASS_PROCESS, /* kernel value */
					  PROCESS__TRANSITION, AVC_STRICT,
					  &dummy_avd);
		if (!rc)
			mysids2[j++] = mysids[i];
		cond_resched();
	}
	rc = 0;
	kfree(mysids);
	*sids = mysids2;
	*nel = j;
out:
	return rc;
}
