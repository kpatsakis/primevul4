static noinline void submit_compressed_extents(struct inode *inode,
					      struct async_cow *async_cow)
{
	struct async_extent *async_extent;
	u64 alloc_hint = 0;
	struct btrfs_key ins;
	struct extent_map *em;
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct extent_map_tree *em_tree = &BTRFS_I(inode)->extent_tree;
	struct extent_io_tree *io_tree;
	int ret = 0;

again:
	while (!list_empty(&async_cow->extents)) {
		async_extent = list_entry(async_cow->extents.next,
					  struct async_extent, list);
		list_del(&async_extent->list);

		io_tree = &BTRFS_I(inode)->io_tree;

retry:
		/* did the compression code fall back to uncompressed IO? */
		if (!async_extent->pages) {
			int page_started = 0;
			unsigned long nr_written = 0;

			lock_extent(io_tree, async_extent->start,
					 async_extent->start +
					 async_extent->ram_size - 1);

			/* allocate blocks */
			ret = cow_file_range(inode, async_cow->locked_page,
					     async_extent->start,
					     async_extent->start +
					     async_extent->ram_size - 1,
					     &page_started, &nr_written, 0);

			/* JDM XXX */

			/*
			 * if page_started, cow_file_range inserted an
			 * inline extent and took care of all the unlocking
			 * and IO for us.  Otherwise, we need to submit
			 * all those pages down to the drive.
			 */
			if (!page_started && !ret)
				extent_write_locked_range(io_tree,
						  inode, async_extent->start,
						  async_extent->start +
						  async_extent->ram_size - 1,
						  btrfs_get_extent,
						  WB_SYNC_ALL);
			else if (ret)
				unlock_page(async_cow->locked_page);
			kfree(async_extent);
			cond_resched();
			continue;
		}

		lock_extent(io_tree, async_extent->start,
			    async_extent->start + async_extent->ram_size - 1);

		ret = btrfs_reserve_extent(root,
					   async_extent->compressed_size,
					   async_extent->compressed_size,
					   0, alloc_hint, &ins, 1, 1);
		if (ret) {
			free_async_extent_pages(async_extent);

			if (ret == -ENOSPC) {
				unlock_extent(io_tree, async_extent->start,
					      async_extent->start +
					      async_extent->ram_size - 1);

				/*
				 * we need to redirty the pages if we decide to
				 * fallback to uncompressed IO, otherwise we
				 * will not submit these pages down to lower
				 * layers.
				 */
				extent_range_redirty_for_io(inode,
						async_extent->start,
						async_extent->start +
						async_extent->ram_size - 1);

				goto retry;
			}
			goto out_free;
		}
		/*
		 * here we're doing allocation and writeback of the
		 * compressed pages
		 */
		btrfs_drop_extent_cache(inode, async_extent->start,
					async_extent->start +
					async_extent->ram_size - 1, 0);

		em = alloc_extent_map();
		if (!em) {
			ret = -ENOMEM;
			goto out_free_reserve;
		}
		em->start = async_extent->start;
		em->len = async_extent->ram_size;
		em->orig_start = em->start;
		em->mod_start = em->start;
		em->mod_len = em->len;

		em->block_start = ins.objectid;
		em->block_len = ins.offset;
		em->orig_block_len = ins.offset;
		em->ram_bytes = async_extent->ram_size;
		em->bdev = root->fs_info->fs_devices->latest_bdev;
		em->compress_type = async_extent->compress_type;
		set_bit(EXTENT_FLAG_PINNED, &em->flags);
		set_bit(EXTENT_FLAG_COMPRESSED, &em->flags);
		em->generation = -1;

		while (1) {
			write_lock(&em_tree->lock);
			ret = add_extent_mapping(em_tree, em, 1);
			write_unlock(&em_tree->lock);
			if (ret != -EEXIST) {
				free_extent_map(em);
				break;
			}
			btrfs_drop_extent_cache(inode, async_extent->start,
						async_extent->start +
						async_extent->ram_size - 1, 0);
		}

		if (ret)
			goto out_free_reserve;

		ret = btrfs_add_ordered_extent_compress(inode,
						async_extent->start,
						ins.objectid,
						async_extent->ram_size,
						ins.offset,
						BTRFS_ORDERED_COMPRESSED,
						async_extent->compress_type);
		if (ret) {
			btrfs_drop_extent_cache(inode, async_extent->start,
						async_extent->start +
						async_extent->ram_size - 1, 0);
			goto out_free_reserve;
		}

		/*
		 * clear dirty, set writeback and unlock the pages.
		 */
		extent_clear_unlock_delalloc(inode, async_extent->start,
				async_extent->start +
				async_extent->ram_size - 1,
				NULL, EXTENT_LOCKED | EXTENT_DELALLOC,
				PAGE_UNLOCK | PAGE_CLEAR_DIRTY |
				PAGE_SET_WRITEBACK);
		ret = btrfs_submit_compressed_write(inode,
				    async_extent->start,
				    async_extent->ram_size,
				    ins.objectid,
				    ins.offset, async_extent->pages,
				    async_extent->nr_pages);
		if (ret) {
			struct extent_io_tree *tree = &BTRFS_I(inode)->io_tree;
			struct page *p = async_extent->pages[0];
			const u64 start = async_extent->start;
			const u64 end = start + async_extent->ram_size - 1;

			p->mapping = inode->i_mapping;
			tree->ops->writepage_end_io_hook(p, start, end,
							 NULL, 0);
			p->mapping = NULL;
			extent_clear_unlock_delalloc(inode, start, end, NULL, 0,
						     PAGE_END_WRITEBACK |
						     PAGE_SET_ERROR);
			free_async_extent_pages(async_extent);
		}
		alloc_hint = ins.objectid + ins.offset;
		kfree(async_extent);
		cond_resched();
	}
	return;
out_free_reserve:
	btrfs_free_reserved_extent(root, ins.objectid, ins.offset, 1);
out_free:
	extent_clear_unlock_delalloc(inode, async_extent->start,
				     async_extent->start +
				     async_extent->ram_size - 1,
				     NULL, EXTENT_LOCKED | EXTENT_DELALLOC |
				     EXTENT_DEFRAG | EXTENT_DO_ACCOUNTING,
				     PAGE_UNLOCK | PAGE_CLEAR_DIRTY |
				     PAGE_SET_WRITEBACK | PAGE_END_WRITEBACK |
				     PAGE_SET_ERROR);
	free_async_extent_pages(async_extent);
	kfree(async_extent);
	goto again;
}
