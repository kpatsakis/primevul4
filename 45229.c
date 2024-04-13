static void aio_free_ring(struct kioctx *ctx)
{
	int i;

	for (i = 0; i < ctx->nr_pages; i++) {
		pr_debug("pid(%d) [%d] page->count=%d\n", current->pid, i,
				page_count(ctx->ring_pages[i]));
		put_page(ctx->ring_pages[i]);
	}

	put_aio_ring_file(ctx);

	if (ctx->ring_pages && ctx->ring_pages != ctx->internal_pages)
		kfree(ctx->ring_pages);
}
