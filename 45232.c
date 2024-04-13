static int aio_setup_ring(struct kioctx *ctx)
{
	struct aio_ring *ring;
	unsigned nr_events = ctx->max_reqs;
	struct mm_struct *mm = current->mm;
	unsigned long size, populate;
	int nr_pages;
	int i;
	struct file *file;

	/* Compensate for the ring buffer's head/tail overlap entry */
	nr_events += 2;	/* 1 is required, 2 for good luck */

	size = sizeof(struct aio_ring);
	size += sizeof(struct io_event) * nr_events;

	nr_pages = PFN_UP(size);
	if (nr_pages < 0)
		return -EINVAL;

	file = anon_inode_getfile_private("[aio]", &aio_ring_fops, ctx, O_RDWR);
	if (IS_ERR(file)) {
		ctx->aio_ring_file = NULL;
		return -EAGAIN;
	}

	file->f_inode->i_mapping->a_ops = &aio_ctx_aops;
	file->f_inode->i_mapping->private_data = ctx;
	file->f_inode->i_size = PAGE_SIZE * (loff_t)nr_pages;

	for (i = 0; i < nr_pages; i++) {
		struct page *page;
		page = find_or_create_page(file->f_inode->i_mapping,
					   i, GFP_HIGHUSER | __GFP_ZERO);
		if (!page)
			break;
		pr_debug("pid(%d) page[%d]->count=%d\n",
			 current->pid, i, page_count(page));
		SetPageUptodate(page);
		SetPageDirty(page);
		unlock_page(page);
	}
	ctx->aio_ring_file = file;
	nr_events = (PAGE_SIZE * nr_pages - sizeof(struct aio_ring))
			/ sizeof(struct io_event);

	ctx->ring_pages = ctx->internal_pages;
	if (nr_pages > AIO_RING_PAGES) {
		ctx->ring_pages = kcalloc(nr_pages, sizeof(struct page *),
					  GFP_KERNEL);
		if (!ctx->ring_pages)
			return -ENOMEM;
	}

	ctx->mmap_size = nr_pages * PAGE_SIZE;
	pr_debug("attempting mmap of %lu bytes\n", ctx->mmap_size);

	down_write(&mm->mmap_sem);
	ctx->mmap_base = do_mmap_pgoff(ctx->aio_ring_file, 0, ctx->mmap_size,
				       PROT_READ | PROT_WRITE,
				       MAP_SHARED | MAP_POPULATE, 0, &populate);
	if (IS_ERR((void *)ctx->mmap_base)) {
		up_write(&mm->mmap_sem);
		ctx->mmap_size = 0;
		aio_free_ring(ctx);
		return -EAGAIN;
	}

	pr_debug("mmap address: 0x%08lx\n", ctx->mmap_base);

	/* We must do this while still holding mmap_sem for write, as we
	 * need to be protected against userspace attempting to mremap()
	 * or munmap() the ring buffer.
	 */
	ctx->nr_pages = get_user_pages(current, mm, ctx->mmap_base, nr_pages,
				       1, 0, ctx->ring_pages, NULL);

	/* Dropping the reference here is safe as the page cache will hold
	 * onto the pages for us.  It is also required so that page migration
	 * can unmap the pages and get the right reference count.
	 */
	for (i = 0; i < ctx->nr_pages; i++)
		put_page(ctx->ring_pages[i]);

	up_write(&mm->mmap_sem);

	if (unlikely(ctx->nr_pages != nr_pages)) {
		aio_free_ring(ctx);
		return -EAGAIN;
	}

	ctx->user_id = ctx->mmap_base;
	ctx->nr_events = nr_events; /* trusted copy */

	ring = kmap_atomic(ctx->ring_pages[0]);
	ring->nr = nr_events;	/* user copy */
	ring->id = ~0U;
	ring->head = ring->tail = 0;
	ring->magic = AIO_RING_MAGIC;
	ring->compat_features = AIO_RING_COMPAT_FEATURES;
	ring->incompat_features = AIO_RING_INCOMPAT_FEATURES;
	ring->header_length = sizeof(struct aio_ring);
	kunmap_atomic(ring);
	flush_dcache_page(ctx->ring_pages[0]);

	return 0;
}
