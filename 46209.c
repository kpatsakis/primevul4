static noinline int cow_file_range(struct inode *inode,
				   struct page *locked_page,
				   u64 start, u64 end, int *page_started,
				   unsigned long *nr_written,
				   int unlock)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;
	u64 alloc_hint = 0;
	u64 num_bytes;
	unsigned long ram_size;
	u64 disk_num_bytes;
	u64 cur_alloc_size;
	u64 blocksize = root->sectorsize;
	struct btrfs_key ins;
	struct extent_map *em;
	struct extent_map_tree *em_tree = &BTRFS_I(inode)->extent_tree;
	int ret = 0;

	if (btrfs_is_free_space_inode(inode)) {
		WARN_ON_ONCE(1);
		ret = -EINVAL;
		goto out_unlock;
	}

	num_bytes = ALIGN(end - start + 1, blocksize);
	num_bytes = max(blocksize,  num_bytes);
	disk_num_bytes = num_bytes;

	/* if this is a small write inside eof, kick off defrag */
	if (num_bytes < 64 * 1024 &&
	    (start > 0 || end + 1 < BTRFS_I(inode)->disk_i_size))
		btrfs_add_inode_defrag(NULL, inode);

	if (start == 0) {
		/* lets try to make an inline extent */
		ret = cow_file_range_inline(root, inode, start, end, 0, 0,
					    NULL);
		if (ret == 0) {
			extent_clear_unlock_delalloc(inode, start, end, NULL,
				     EXTENT_LOCKED | EXTENT_DELALLOC |
				     EXTENT_DEFRAG, PAGE_UNLOCK |
				     PAGE_CLEAR_DIRTY | PAGE_SET_WRITEBACK |
				     PAGE_END_WRITEBACK);

			*nr_written = *nr_written +
			     (end - start + PAGE_CACHE_SIZE) / PAGE_CACHE_SIZE;
			*page_started = 1;
			goto out;
		} else if (ret < 0) {
			goto out_unlock;
		}
	}

	BUG_ON(disk_num_bytes >
	       btrfs_super_total_bytes(root->fs_info->super_copy));

	alloc_hint = get_extent_allocation_hint(inode, start, num_bytes);
	btrfs_drop_extent_cache(inode, start, start + num_bytes - 1, 0);

	while (disk_num_bytes > 0) {
		unsigned long op;

		cur_alloc_size = disk_num_bytes;
		ret = btrfs_reserve_extent(root, cur_alloc_size,
					   root->sectorsize, 0, alloc_hint,
					   &ins, 1, 1);
		if (ret < 0)
			goto out_unlock;

		em = alloc_extent_map();
		if (!em) {
			ret = -ENOMEM;
			goto out_reserve;
		}
		em->start = start;
		em->orig_start = em->start;
		ram_size = ins.offset;
		em->len = ins.offset;
		em->mod_start = em->start;
		em->mod_len = em->len;

		em->block_start = ins.objectid;
		em->block_len = ins.offset;
		em->orig_block_len = ins.offset;
		em->ram_bytes = ram_size;
		em->bdev = root->fs_info->fs_devices->latest_bdev;
		set_bit(EXTENT_FLAG_PINNED, &em->flags);
		em->generation = -1;

		while (1) {
			write_lock(&em_tree->lock);
			ret = add_extent_mapping(em_tree, em, 1);
			write_unlock(&em_tree->lock);
			if (ret != -EEXIST) {
				free_extent_map(em);
				break;
			}
			btrfs_drop_extent_cache(inode, start,
						start + ram_size - 1, 0);
		}
		if (ret)
			goto out_reserve;

		cur_alloc_size = ins.offset;
		ret = btrfs_add_ordered_extent(inode, start, ins.objectid,
					       ram_size, cur_alloc_size, 0);
		if (ret)
			goto out_drop_extent_cache;

		if (root->root_key.objectid ==
		    BTRFS_DATA_RELOC_TREE_OBJECTID) {
			ret = btrfs_reloc_clone_csums(inode, start,
						      cur_alloc_size);
			if (ret)
				goto out_drop_extent_cache;
		}

		if (disk_num_bytes < cur_alloc_size)
			break;

		/* we're not doing compressed IO, don't unlock the first
		 * page (which the caller expects to stay locked), don't
		 * clear any dirty bits and don't set any writeback bits
		 *
		 * Do set the Private2 bit so we know this page was properly
		 * setup for writepage
		 */
		op = unlock ? PAGE_UNLOCK : 0;
		op |= PAGE_SET_PRIVATE2;

		extent_clear_unlock_delalloc(inode, start,
					     start + ram_size - 1, locked_page,
					     EXTENT_LOCKED | EXTENT_DELALLOC,
					     op);
		disk_num_bytes -= cur_alloc_size;
		num_bytes -= cur_alloc_size;
		alloc_hint = ins.objectid + ins.offset;
		start += cur_alloc_size;
	}
out:
	return ret;

out_drop_extent_cache:
	btrfs_drop_extent_cache(inode, start, start + ram_size - 1, 0);
out_reserve:
	btrfs_free_reserved_extent(root, ins.objectid, ins.offset, 1);
out_unlock:
	extent_clear_unlock_delalloc(inode, start, end, locked_page,
				     EXTENT_LOCKED | EXTENT_DO_ACCOUNTING |
				     EXTENT_DELALLOC | EXTENT_DEFRAG,
				     PAGE_UNLOCK | PAGE_CLEAR_DIRTY |
				     PAGE_SET_WRITEBACK | PAGE_END_WRITEBACK);
	goto out;
}
