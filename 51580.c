static void unroll_tree_refs(struct audit_context *ctx,
		      struct audit_tree_refs *p, int count)
{
#ifdef CONFIG_AUDIT_TREE
	struct audit_tree_refs *q;
	int n;
	if (!p) {
		/* we started with empty chain */
		p = ctx->first_trees;
		count = 31;
		/* if the very first allocation has failed, nothing to do */
		if (!p)
			return;
	}
	n = count;
	for (q = p; q != ctx->trees; q = q->next, n = 31) {
		while (n--) {
			audit_put_chunk(q->c[n]);
			q->c[n] = NULL;
		}
	}
	while (n-- > ctx->tree_count) {
		audit_put_chunk(q->c[n]);
		q->c[n] = NULL;
	}
	ctx->trees = p;
	ctx->tree_count = count;
#endif
}
