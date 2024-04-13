static int scan(struct ctl_table_header *head, struct ctl_table *table,
		unsigned long *pos, struct file *file,
		struct dir_context *ctx)
{
	bool res;

	if ((*pos)++ < ctx->pos)
		return true;

	if (unlikely(S_ISLNK(table->mode)))
		res = proc_sys_link_fill_cache(file, ctx, head, table);
	else
		res = proc_sys_fill_cache(file, ctx, head, table);

	if (res)
		ctx->pos = *pos;

	return res;
}
