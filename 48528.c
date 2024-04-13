void lrw_free_table(struct lrw_table_ctx *ctx)
{
	if (ctx->table)
		gf128mul_free_64k(ctx->table);
}
