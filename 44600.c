static int context_struct_to_string(struct context *context, char **scontext, u32 *scontext_len)
{
	char *scontextp;

	if (scontext)
		*scontext = NULL;
	*scontext_len = 0;

	if (context->len) {
		*scontext_len = context->len;
		if (scontext) {
			*scontext = kstrdup(context->str, GFP_ATOMIC);
			if (!(*scontext))
				return -ENOMEM;
		}
		return 0;
	}

	/* Compute the size of the context. */
	*scontext_len += strlen(sym_name(&policydb, SYM_USERS, context->user - 1)) + 1;
	*scontext_len += strlen(sym_name(&policydb, SYM_ROLES, context->role - 1)) + 1;
	*scontext_len += strlen(sym_name(&policydb, SYM_TYPES, context->type - 1)) + 1;
	*scontext_len += mls_compute_context_len(context);

	if (!scontext)
		return 0;

	/* Allocate space for the context; caller must free this space. */
	scontextp = kmalloc(*scontext_len, GFP_ATOMIC);
	if (!scontextp)
		return -ENOMEM;
	*scontext = scontextp;

	/*
	 * Copy the user name, role name and type name into the context.
	 */
	sprintf(scontextp, "%s:%s:%s",
		sym_name(&policydb, SYM_USERS, context->user - 1),
		sym_name(&policydb, SYM_ROLES, context->role - 1),
		sym_name(&policydb, SYM_TYPES, context->type - 1));
	scontextp += strlen(sym_name(&policydb, SYM_USERS, context->user - 1)) +
		     1 + strlen(sym_name(&policydb, SYM_ROLES, context->role - 1)) +
		     1 + strlen(sym_name(&policydb, SYM_TYPES, context->type - 1));

	mls_sid_to_context(context, &scontextp);

	*scontextp = 0;

	return 0;
}
