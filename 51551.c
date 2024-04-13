static int audit_compare_gid(kgid_t gid,
			     struct audit_names *name,
			     struct audit_field *f,
			     struct audit_context *ctx)
{
	struct audit_names *n;
	int rc;
 
	if (name) {
		rc = audit_gid_comparator(gid, f->op, name->gid);
		if (rc)
			return rc;
	}
 
	if (ctx) {
		list_for_each_entry(n, &ctx->names_list, list) {
			rc = audit_gid_comparator(gid, f->op, n->gid);
			if (rc)
				return rc;
		}
	}
	return 0;
}
