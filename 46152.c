static int btrfs_get_blocks_direct(struct inode *inode, sector_t iblock,
				   struct buffer_head *bh_result, int create)
{
	struct extent_map *em;
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct extent_state *cached_state = NULL;
	struct btrfs_dio_data *dio_data = NULL;
	u64 start = iblock << inode->i_blkbits;
	u64 lockstart, lockend;
	u64 len = bh_result->b_size;
	int unlock_bits = EXTENT_LOCKED;
	int ret = 0;

	if (create)
		unlock_bits |= EXTENT_DIRTY;
	else
		len = min_t(u64, len, root->sectorsize);

	lockstart = start;
	lockend = start + len - 1;

	if (current->journal_info) {
		/*
		 * Need to pull our outstanding extents and set journal_info to NULL so
		 * that anything that needs to check if there's a transction doesn't get
		 * confused.
		 */
		dio_data = current->journal_info;
		current->journal_info = NULL;
	}

	/*
	 * If this errors out it's because we couldn't invalidate pagecache for
	 * this range and we need to fallback to buffered.
	 */
	if (lock_extent_direct(inode, lockstart, lockend, &cached_state, create))
		return -ENOTBLK;

	em = btrfs_get_extent(inode, NULL, 0, start, len, 0);
	if (IS_ERR(em)) {
		ret = PTR_ERR(em);
		goto unlock_err;
	}

	/*
	 * Ok for INLINE and COMPRESSED extents we need to fallback on buffered
	 * io.  INLINE is special, and we could probably kludge it in here, but
	 * it's still buffered so for safety lets just fall back to the generic
	 * buffered path.
	 *
	 * For COMPRESSED we _have_ to read the entire extent in so we can
	 * decompress it, so there will be buffering required no matter what we
	 * do, so go ahead and fallback to buffered.
	 *
	 * We return -ENOTBLK because thats what makes DIO go ahead and go back
	 * to buffered IO.  Don't blame me, this is the price we pay for using
	 * the generic code.
	 */
	if (test_bit(EXTENT_FLAG_COMPRESSED, &em->flags) ||
	    em->block_start == EXTENT_MAP_INLINE) {
		free_extent_map(em);
		ret = -ENOTBLK;
		goto unlock_err;
	}

	/* Just a good old fashioned hole, return */
	if (!create && (em->block_start == EXTENT_MAP_HOLE ||
			test_bit(EXTENT_FLAG_PREALLOC, &em->flags))) {
		free_extent_map(em);
		goto unlock_err;
	}

	/*
	 * We don't allocate a new extent in the following cases
	 *
	 * 1) The inode is marked as NODATACOW.  In this case we'll just use the
	 * existing extent.
	 * 2) The extent is marked as PREALLOC.  We're good to go here and can
	 * just use the extent.
	 *
	 */
	if (!create) {
		len = min(len, em->len - (start - em->start));
		lockstart = start + len;
		goto unlock;
	}

	if (test_bit(EXTENT_FLAG_PREALLOC, &em->flags) ||
	    ((BTRFS_I(inode)->flags & BTRFS_INODE_NODATACOW) &&
	     em->block_start != EXTENT_MAP_HOLE)) {
		int type;
		u64 block_start, orig_start, orig_block_len, ram_bytes;

		if (test_bit(EXTENT_FLAG_PREALLOC, &em->flags))
			type = BTRFS_ORDERED_PREALLOC;
		else
			type = BTRFS_ORDERED_NOCOW;
		len = min(len, em->len - (start - em->start));
		block_start = em->block_start + (start - em->start);

		if (can_nocow_extent(inode, start, &len, &orig_start,
				     &orig_block_len, &ram_bytes) == 1) {
			if (type == BTRFS_ORDERED_PREALLOC) {
				free_extent_map(em);
				em = create_pinned_em(inode, start, len,
						       orig_start,
						       block_start, len,
						       orig_block_len,
						       ram_bytes, type);
				if (IS_ERR(em)) {
					ret = PTR_ERR(em);
					goto unlock_err;
				}
			}

			ret = btrfs_add_ordered_extent_dio(inode, start,
					   block_start, len, len, type);
			if (ret) {
				free_extent_map(em);
				goto unlock_err;
			}
			goto unlock;
		}
	}

	/*
	 * this will cow the extent, reset the len in case we changed
	 * it above
	 */
	len = bh_result->b_size;
	free_extent_map(em);
	em = btrfs_new_extent_direct(inode, start, len);
	if (IS_ERR(em)) {
		ret = PTR_ERR(em);
		goto unlock_err;
	}
	len = min(len, em->len - (start - em->start));
unlock:
	bh_result->b_blocknr = (em->block_start + (start - em->start)) >>
		inode->i_blkbits;
	bh_result->b_size = len;
	bh_result->b_bdev = em->bdev;
	set_buffer_mapped(bh_result);
	if (create) {
		if (!test_bit(EXTENT_FLAG_PREALLOC, &em->flags))
			set_buffer_new(bh_result);

		/*
		 * Need to update the i_size under the extent lock so buffered
		 * readers will get the updated i_size when we unlock.
		 */
		if (start + len > i_size_read(inode))
			i_size_write(inode, start + len);

		/*
		 * If we have an outstanding_extents count still set then we're
		 * within our reservation, otherwise we need to adjust our inode
		 * counter appropriately.
		 */
		if (dio_data->outstanding_extents) {
			(dio_data->outstanding_extents)--;
		} else {
			spin_lock(&BTRFS_I(inode)->lock);
			BTRFS_I(inode)->outstanding_extents++;
			spin_unlock(&BTRFS_I(inode)->lock);
		}

		btrfs_free_reserved_data_space(inode, len);
		WARN_ON(dio_data->reserve < len);
		dio_data->reserve -= len;
		current->journal_info = dio_data;
	}

	/*
	 * In the case of write we need to clear and unlock the entire range,
	 * in the case of read we need to unlock only the end area that we
	 * aren't using if there is any left over space.
	 */
	if (lockstart < lockend) {
		clear_extent_bit(&BTRFS_I(inode)->io_tree, lockstart,
				 lockend, unlock_bits, 1, 0,
				 &cached_state, GFP_NOFS);
	} else {
		free_extent_state(cached_state);
	}

	free_extent_map(em);

	return 0;

unlock_err:
	clear_extent_bit(&BTRFS_I(inode)->io_tree, lockstart, lockend,
			 unlock_bits, 1, 0, &cached_state, GFP_NOFS);
	if (dio_data)
		current->journal_info = dio_data;
	return ret;
}
