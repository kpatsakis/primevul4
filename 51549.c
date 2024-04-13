static inline struct audit_context *audit_alloc_context(enum audit_state state)
{
	struct audit_context *context;

	context = kzalloc(sizeof(*context), GFP_KERNEL);
	if (!context)
		return NULL;
	context->state = state;
	context->prio = state == AUDIT_RECORD_CONTEXT ? ~0ULL : 0;
	INIT_LIST_HEAD(&context->killed_trees);
	INIT_LIST_HEAD(&context->names_list);
	return context;
}
