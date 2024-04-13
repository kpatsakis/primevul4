int security_validate_transition(u32 oldsid, u32 newsid, u32 tasksid,
				 u16 orig_tclass)
{
	struct context *ocontext;
	struct context *ncontext;
	struct context *tcontext;
	struct class_datum *tclass_datum;
	struct constraint_node *constraint;
	u16 tclass;
	int rc = 0;

	if (!ss_initialized)
		return 0;

	read_lock(&policy_rwlock);

	tclass = unmap_class(orig_tclass);

	if (!tclass || tclass > policydb.p_classes.nprim) {
		printk(KERN_ERR "SELinux: %s:  unrecognized class %d\n",
			__func__, tclass);
		rc = -EINVAL;
		goto out;
	}
	tclass_datum = policydb.class_val_to_struct[tclass - 1];

	ocontext = sidtab_search(&sidtab, oldsid);
	if (!ocontext) {
		printk(KERN_ERR "SELinux: %s:  unrecognized SID %d\n",
			__func__, oldsid);
		rc = -EINVAL;
		goto out;
	}

	ncontext = sidtab_search(&sidtab, newsid);
	if (!ncontext) {
		printk(KERN_ERR "SELinux: %s:  unrecognized SID %d\n",
			__func__, newsid);
		rc = -EINVAL;
		goto out;
	}

	tcontext = sidtab_search(&sidtab, tasksid);
	if (!tcontext) {
		printk(KERN_ERR "SELinux: %s:  unrecognized SID %d\n",
			__func__, tasksid);
		rc = -EINVAL;
		goto out;
	}

	constraint = tclass_datum->validatetrans;
	while (constraint) {
		if (!constraint_expr_eval(ocontext, ncontext, tcontext,
					  constraint->expr)) {
			rc = security_validtrans_handle_fail(ocontext, ncontext,
							     tcontext, tclass);
			goto out;
		}
		constraint = constraint->next;
	}

out:
	read_unlock(&policy_rwlock);
	return rc;
}
