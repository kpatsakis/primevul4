static int audit_compare_uid(kuid_t uid,
			     struct audit_names *name,
			     struct audit_field *f,
			     struct audit_context *ctx)
{
	struct audit_names *n;
	int rc;
 
	if (name) {
		rc = audit_uid_comparator(uid, f->op, name->uid);
		if (rc)
			return rc;
	}
 
	if (ctx) {
		list_for_each_entry(n, &ctx->names_list, list) {
			rc = audit_uid_comparator(uid, f->op, n->uid);
			if (rc)
				return rc;
		}
	}
	return 0;
}
