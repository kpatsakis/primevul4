static void security_dump_masked_av(struct context *scontext,
				    struct context *tcontext,
				    u16 tclass,
				    u32 permissions,
				    const char *reason)
{
	struct common_datum *common_dat;
	struct class_datum *tclass_dat;
	struct audit_buffer *ab;
	char *tclass_name;
	char *scontext_name = NULL;
	char *tcontext_name = NULL;
	char *permission_names[32];
	int index;
	u32 length;
	bool need_comma = false;

	if (!permissions)
		return;

	tclass_name = sym_name(&policydb, SYM_CLASSES, tclass - 1);
	tclass_dat = policydb.class_val_to_struct[tclass - 1];
	common_dat = tclass_dat->comdatum;

	/* init permission_names */
	if (common_dat &&
	    hashtab_map(common_dat->permissions.table,
			dump_masked_av_helper, permission_names) < 0)
		goto out;

	if (hashtab_map(tclass_dat->permissions.table,
			dump_masked_av_helper, permission_names) < 0)
		goto out;

	/* get scontext/tcontext in text form */
	if (context_struct_to_string(scontext,
				     &scontext_name, &length) < 0)
		goto out;

	if (context_struct_to_string(tcontext,
				     &tcontext_name, &length) < 0)
		goto out;

	/* audit a message */
	ab = audit_log_start(current->audit_context,
			     GFP_ATOMIC, AUDIT_SELINUX_ERR);
	if (!ab)
		goto out;

	audit_log_format(ab, "op=security_compute_av reason=%s "
			 "scontext=%s tcontext=%s tclass=%s perms=",
			 reason, scontext_name, tcontext_name, tclass_name);

	for (index = 0; index < 32; index++) {
		u32 mask = (1 << index);

		if ((mask & permissions) == 0)
			continue;

		audit_log_format(ab, "%s%s",
				 need_comma ? "," : "",
				 permission_names[index]
				 ? permission_names[index] : "????");
		need_comma = true;
	}
	audit_log_end(ab);
out:
	/* release scontext/tcontext */
	kfree(tcontext_name);
	kfree(scontext_name);

	return;
}
