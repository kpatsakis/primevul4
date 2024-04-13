int security_set_bools(int len, int *values)
{
	int i, rc;
	int lenp, seqno = 0;
	struct cond_node *cur;

	write_lock_irq(&policy_rwlock);

	rc = -EFAULT;
	lenp = policydb.p_bools.nprim;
	if (len != lenp)
		goto out;

	for (i = 0; i < len; i++) {
		if (!!values[i] != policydb.bool_val_to_struct[i]->state) {
			audit_log(current->audit_context, GFP_ATOMIC,
				AUDIT_MAC_CONFIG_CHANGE,
				"bool=%s val=%d old_val=%d auid=%u ses=%u",
				sym_name(&policydb, SYM_BOOLS, i),
				!!values[i],
				policydb.bool_val_to_struct[i]->state,
				from_kuid(&init_user_ns, audit_get_loginuid(current)),
				audit_get_sessionid(current));
		}
		if (values[i])
			policydb.bool_val_to_struct[i]->state = 1;
		else
			policydb.bool_val_to_struct[i]->state = 0;
	}

	for (cur = policydb.cond_list; cur; cur = cur->next) {
		rc = evaluate_cond_node(&policydb, cur);
		if (rc)
			goto out;
	}

	seqno = ++latest_granting;
	rc = 0;
out:
	write_unlock_irq(&policy_rwlock);
	if (!rc) {
		avc_ss_reset(seqno);
		selnl_notify_policyload(seqno);
		selinux_status_update_policyload(seqno);
		selinux_xfrm_notify_policyload();
	}
	return rc;
}
