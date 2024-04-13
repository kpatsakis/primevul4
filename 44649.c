static int string_to_context_struct(struct policydb *pol,
				    struct sidtab *sidtabp,
				    char *scontext,
				    u32 scontext_len,
				    struct context *ctx,
				    u32 def_sid)
{
	struct role_datum *role;
	struct type_datum *typdatum;
	struct user_datum *usrdatum;
	char *scontextp, *p, oldc;
	int rc = 0;

	context_init(ctx);

	/* Parse the security context. */

	rc = -EINVAL;
	scontextp = (char *) scontext;

	/* Extract the user. */
	p = scontextp;
	while (*p && *p != ':')
		p++;

	if (*p == 0)
		goto out;

	*p++ = 0;

	usrdatum = hashtab_search(pol->p_users.table, scontextp);
	if (!usrdatum)
		goto out;

	ctx->user = usrdatum->value;

	/* Extract role. */
	scontextp = p;
	while (*p && *p != ':')
		p++;

	if (*p == 0)
		goto out;

	*p++ = 0;

	role = hashtab_search(pol->p_roles.table, scontextp);
	if (!role)
		goto out;
	ctx->role = role->value;

	/* Extract type. */
	scontextp = p;
	while (*p && *p != ':')
		p++;
	oldc = *p;
	*p++ = 0;

	typdatum = hashtab_search(pol->p_types.table, scontextp);
	if (!typdatum || typdatum->attribute)
		goto out;

	ctx->type = typdatum->value;

	rc = mls_context_to_sid(pol, oldc, &p, ctx, sidtabp, def_sid);
	if (rc)
		goto out;

	rc = -EINVAL;
	if ((p - scontext) < scontext_len)
		goto out;

	/* Check the validity of the new context. */
	if (!policydb_context_isvalid(pol, ctx))
		goto out;
	rc = 0;
out:
	if (rc)
		context_destroy(ctx);
	return rc;
}
