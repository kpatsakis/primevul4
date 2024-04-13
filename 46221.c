static int insert_inline_extent(struct btrfs_trans_handle *trans,
				struct btrfs_path *path, int extent_inserted,
				struct btrfs_root *root, struct inode *inode,
				u64 start, size_t size, size_t compressed_size,
				int compress_type,
				struct page **compressed_pages)
{
	struct extent_buffer *leaf;
	struct page *page = NULL;
	char *kaddr;
	unsigned long ptr;
	struct btrfs_file_extent_item *ei;
	int err = 0;
	int ret;
	size_t cur_size = size;
	unsigned long offset;

	if (compressed_size && compressed_pages)
		cur_size = compressed_size;

	inode_add_bytes(inode, size);

	if (!extent_inserted) {
		struct btrfs_key key;
		size_t datasize;

		key.objectid = btrfs_ino(inode);
		key.offset = start;
		key.type = BTRFS_EXTENT_DATA_KEY;

		datasize = btrfs_file_extent_calc_inline_size(cur_size);
		path->leave_spinning = 1;
		ret = btrfs_insert_empty_item(trans, root, path, &key,
					      datasize);
		if (ret) {
			err = ret;
			goto fail;
		}
	}
	leaf = path->nodes[0];
	ei = btrfs_item_ptr(leaf, path->slots[0],
			    struct btrfs_file_extent_item);
	btrfs_set_file_extent_generation(leaf, ei, trans->transid);
	btrfs_set_file_extent_type(leaf, ei, BTRFS_FILE_EXTENT_INLINE);
	btrfs_set_file_extent_encryption(leaf, ei, 0);
	btrfs_set_file_extent_other_encoding(leaf, ei, 0);
	btrfs_set_file_extent_ram_bytes(leaf, ei, size);
	ptr = btrfs_file_extent_inline_start(ei);

	if (compress_type != BTRFS_COMPRESS_NONE) {
		struct page *cpage;
		int i = 0;
		while (compressed_size > 0) {
			cpage = compressed_pages[i];
			cur_size = min_t(unsigned long, compressed_size,
				       PAGE_CACHE_SIZE);

			kaddr = kmap_atomic(cpage);
			write_extent_buffer(leaf, kaddr, ptr, cur_size);
			kunmap_atomic(kaddr);

			i++;
			ptr += cur_size;
			compressed_size -= cur_size;
		}
		btrfs_set_file_extent_compression(leaf, ei,
						  compress_type);
	} else {
		page = find_get_page(inode->i_mapping,
				     start >> PAGE_CACHE_SHIFT);
		btrfs_set_file_extent_compression(leaf, ei, 0);
		kaddr = kmap_atomic(page);
		offset = start & (PAGE_CACHE_SIZE - 1);
		write_extent_buffer(leaf, kaddr + offset, ptr, size);
		kunmap_atomic(kaddr);
		page_cache_release(page);
	}
	btrfs_mark_buffer_dirty(leaf);
	btrfs_release_path(path);

	/*
	 * we're an inline extent, so nobody can
	 * extend the file past i_size without locking
	 * a page we already have locked.
	 *
	 * We must do any isize and inode updates
	 * before we unlock the pages.  Otherwise we
	 * could end up racing with unlink.
	 */
	BTRFS_I(inode)->disk_i_size = inode->i_size;
	ret = btrfs_update_inode(trans, root, inode);

	return ret;
fail:
	return err;
}
