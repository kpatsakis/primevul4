static inline void audit_free_context(struct audit_context *context)
{
	audit_free_names(context);
	unroll_tree_refs(context, NULL, 0);
	free_tree_refs(context);
	audit_free_aux(context);
	kfree(context->filterkey);
	kfree(context->sockaddr);
	audit_proctitle_free(context);
	kfree(context);
}
