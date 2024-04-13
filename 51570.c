static inline void audit_proctitle_free(struct audit_context *context)
{
	kfree(context->proctitle.value);
	context->proctitle.value = NULL;
	context->proctitle.len = 0;
}
