static int aio_migratepage(struct address_space *mapping, struct page *new,
			struct page *old, enum migrate_mode mode)
{
	struct kioctx *ctx;
	unsigned long flags;
	int rc;

	/* Writeback must be complete */
	BUG_ON(PageWriteback(old));
	put_page(old);

	rc = migrate_page_move_mapping(mapping, new, old, NULL, mode);
	if (rc != MIGRATEPAGE_SUCCESS) {
		get_page(old);
		return rc;
	}

	get_page(new);

	/* We can potentially race against kioctx teardown here.  Use the
	 * address_space's private data lock to protect the mapping's
	 * private_data.
	 */
	spin_lock(&mapping->private_lock);
	ctx = mapping->private_data;
	if (ctx) {
		pgoff_t idx;
		spin_lock_irqsave(&ctx->completion_lock, flags);
		migrate_page_copy(new, old);
		idx = old->index;
		if (idx < (pgoff_t)ctx->nr_pages)
			ctx->ring_pages[idx] = new;
		spin_unlock_irqrestore(&ctx->completion_lock, flags);
	} else
		rc = -EBUSY;
	spin_unlock(&mapping->private_lock);

	return rc;
}
