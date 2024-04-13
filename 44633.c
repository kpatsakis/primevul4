int security_netlbl_sid_to_secattr(u32 sid, struct netlbl_lsm_secattr *secattr)
{
	int rc;
	struct context *ctx;

	if (!ss_initialized)
		return 0;

	read_lock(&policy_rwlock);

	rc = -ENOENT;
	ctx = sidtab_search(&sidtab, sid);
	if (ctx == NULL)
		goto out;

	rc = -ENOMEM;
	secattr->domain = kstrdup(sym_name(&policydb, SYM_TYPES, ctx->type - 1),
				  GFP_ATOMIC);
	if (secattr->domain == NULL)
		goto out;

	secattr->attr.secid = sid;
	secattr->flags |= NETLBL_SECATTR_DOMAIN_CPY | NETLBL_SECATTR_SECID;
	mls_export_netlbl_lvl(ctx, secattr);
	rc = mls_export_netlbl_cat(ctx, secattr);
out:
	read_unlock(&policy_rwlock);
	return rc;
}
