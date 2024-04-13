static sector_t inode_getblk(struct inode *inode, sector_t block,
			     int *err, int *new)
{
	struct kernel_long_ad laarr[EXTENT_MERGE_SIZE];
	struct extent_position prev_epos, cur_epos, next_epos;
	int count = 0, startnum = 0, endnum = 0;
	uint32_t elen = 0, tmpelen;
	struct kernel_lb_addr eloc, tmpeloc;
	int c = 1;
	loff_t lbcount = 0, b_off = 0;
	uint32_t newblocknum, newblock;
	sector_t offset = 0;
	int8_t etype;
	struct udf_inode_info *iinfo = UDF_I(inode);
	int goal = 0, pgoal = iinfo->i_location.logicalBlockNum;
	int lastblock = 0;
	bool isBeyondEOF;

	*err = 0;
	*new = 0;
	prev_epos.offset = udf_file_entry_alloc_offset(inode);
	prev_epos.block = iinfo->i_location;
	prev_epos.bh = NULL;
	cur_epos = next_epos = prev_epos;
	b_off = (loff_t)block << inode->i_sb->s_blocksize_bits;

	/* find the extent which contains the block we are looking for.
	   alternate between laarr[0] and laarr[1] for locations of the
	   current extent, and the previous extent */
	do {
		if (prev_epos.bh != cur_epos.bh) {
			brelse(prev_epos.bh);
			get_bh(cur_epos.bh);
			prev_epos.bh = cur_epos.bh;
		}
		if (cur_epos.bh != next_epos.bh) {
			brelse(cur_epos.bh);
			get_bh(next_epos.bh);
			cur_epos.bh = next_epos.bh;
		}

		lbcount += elen;

		prev_epos.block = cur_epos.block;
		cur_epos.block = next_epos.block;

		prev_epos.offset = cur_epos.offset;
		cur_epos.offset = next_epos.offset;

		etype = udf_next_aext(inode, &next_epos, &eloc, &elen, 1);
		if (etype == -1)
			break;

		c = !c;

		laarr[c].extLength = (etype << 30) | elen;
		laarr[c].extLocation = eloc;

		if (etype != (EXT_NOT_RECORDED_NOT_ALLOCATED >> 30))
			pgoal = eloc.logicalBlockNum +
				((elen + inode->i_sb->s_blocksize - 1) >>
				 inode->i_sb->s_blocksize_bits);

		count++;
	} while (lbcount + elen <= b_off);

	b_off -= lbcount;
	offset = b_off >> inode->i_sb->s_blocksize_bits;
	/*
	 * Move prev_epos and cur_epos into indirect extent if we are at
	 * the pointer to it
	 */
	udf_next_aext(inode, &prev_epos, &tmpeloc, &tmpelen, 0);
	udf_next_aext(inode, &cur_epos, &tmpeloc, &tmpelen, 0);

	/* if the extent is allocated and recorded, return the block
	   if the extent is not a multiple of the blocksize, round up */

	if (etype == (EXT_RECORDED_ALLOCATED >> 30)) {
		if (elen & (inode->i_sb->s_blocksize - 1)) {
			elen = EXT_RECORDED_ALLOCATED |
				((elen + inode->i_sb->s_blocksize - 1) &
				 ~(inode->i_sb->s_blocksize - 1));
			udf_write_aext(inode, &cur_epos, &eloc, elen, 1);
		}
		brelse(prev_epos.bh);
		brelse(cur_epos.bh);
		brelse(next_epos.bh);
		newblock = udf_get_lb_pblock(inode->i_sb, &eloc, offset);
		return newblock;
	}

	/* Are we beyond EOF? */
	if (etype == -1) {
		int ret;
		isBeyondEOF = 1;
		if (count) {
			if (c)
				laarr[0] = laarr[1];
			startnum = 1;
		} else {
			/* Create a fake extent when there's not one */
			memset(&laarr[0].extLocation, 0x00,
				sizeof(struct kernel_lb_addr));
			laarr[0].extLength = EXT_NOT_RECORDED_NOT_ALLOCATED;
			/* Will udf_do_extend_file() create real extent from
			   a fake one? */
			startnum = (offset > 0);
		}
		/* Create extents for the hole between EOF and offset */
		ret = udf_do_extend_file(inode, &prev_epos, laarr, offset);
		if (ret < 0) {
			brelse(prev_epos.bh);
			brelse(cur_epos.bh);
			brelse(next_epos.bh);
			*err = ret;
			return 0;
		}
		c = 0;
		offset = 0;
		count += ret;
		/* We are not covered by a preallocated extent? */
		if ((laarr[0].extLength & UDF_EXTENT_FLAG_MASK) !=
						EXT_NOT_RECORDED_ALLOCATED) {
			/* Is there any real extent? - otherwise we overwrite
			 * the fake one... */
			if (count)
				c = !c;
			laarr[c].extLength = EXT_NOT_RECORDED_NOT_ALLOCATED |
				inode->i_sb->s_blocksize;
			memset(&laarr[c].extLocation, 0x00,
				sizeof(struct kernel_lb_addr));
			count++;
		}
		endnum = c + 1;
		lastblock = 1;
	} else {
		isBeyondEOF = 0;
		endnum = startnum = ((count > 2) ? 2 : count);

		/* if the current extent is in position 0,
		   swap it with the previous */
		if (!c && count != 1) {
			laarr[2] = laarr[0];
			laarr[0] = laarr[1];
			laarr[1] = laarr[2];
			c = 1;
		}

		/* if the current block is located in an extent,
		   read the next extent */
		etype = udf_next_aext(inode, &next_epos, &eloc, &elen, 0);
		if (etype != -1) {
			laarr[c + 1].extLength = (etype << 30) | elen;
			laarr[c + 1].extLocation = eloc;
			count++;
			startnum++;
			endnum++;
		} else
			lastblock = 1;
	}

	/* if the current extent is not recorded but allocated, get the
	 * block in the extent corresponding to the requested block */
	if ((laarr[c].extLength >> 30) == (EXT_NOT_RECORDED_ALLOCATED >> 30))
		newblocknum = laarr[c].extLocation.logicalBlockNum + offset;
	else { /* otherwise, allocate a new block */
		if (iinfo->i_next_alloc_block == block)
			goal = iinfo->i_next_alloc_goal;

		if (!goal) {
			if (!(goal = pgoal)) /* XXX: what was intended here? */
				goal = iinfo->i_location.logicalBlockNum + 1;
		}

		newblocknum = udf_new_block(inode->i_sb, inode,
				iinfo->i_location.partitionReferenceNum,
				goal, err);
		if (!newblocknum) {
			brelse(prev_epos.bh);
			brelse(cur_epos.bh);
			brelse(next_epos.bh);
			*err = -ENOSPC;
			return 0;
		}
		if (isBeyondEOF)
			iinfo->i_lenExtents += inode->i_sb->s_blocksize;
	}

	/* if the extent the requsted block is located in contains multiple
	 * blocks, split the extent into at most three extents. blocks prior
	 * to requested block, requested block, and blocks after requested
	 * block */
	udf_split_extents(inode, &c, offset, newblocknum, laarr, &endnum);

#ifdef UDF_PREALLOCATE
	/* We preallocate blocks only for regular files. It also makes sense
	 * for directories but there's a problem when to drop the
	 * preallocation. We might use some delayed work for that but I feel
	 * it's overengineering for a filesystem like UDF. */
	if (S_ISREG(inode->i_mode))
		udf_prealloc_extents(inode, c, lastblock, laarr, &endnum);
#endif

	/* merge any continuous blocks in laarr */
	udf_merge_extents(inode, laarr, &endnum);

	/* write back the new extents, inserting new extents if the new number
	 * of extents is greater than the old number, and deleting extents if
	 * the new number of extents is less than the old number */
	udf_update_extents(inode, laarr, startnum, endnum, &prev_epos);

	brelse(prev_epos.bh);
	brelse(cur_epos.bh);
	brelse(next_epos.bh);

	newblock = udf_get_pblock(inode->i_sb, newblocknum,
				iinfo->i_location.partitionReferenceNum, 0);
	if (!newblock) {
		*err = -EIO;
		return 0;
	}
	*new = 1;
	iinfo->i_next_alloc_block = block;
	iinfo->i_next_alloc_goal = newblocknum;
	inode->i_ctime = current_fs_time(inode->i_sb);

	if (IS_SYNC(inode))
		udf_sync_inode(inode);
	else
		mark_inode_dirty(inode);

	return newblock;
}
