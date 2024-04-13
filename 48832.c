static loff_t ext4_seek_hole(struct file *file, loff_t offset, loff_t maxsize)
{
	struct inode *inode = file->f_mapping->host;
	struct ext4_map_blocks map;
	struct extent_status es;
	ext4_lblk_t start, last, end;
	loff_t holeoff, isize;
	int blkbits;
	int ret = 0;

	mutex_lock(&inode->i_mutex);

	isize = i_size_read(inode);
	if (offset >= isize) {
		mutex_unlock(&inode->i_mutex);
		return -ENXIO;
	}

	blkbits = inode->i_sb->s_blocksize_bits;
	start = offset >> blkbits;
	last = start;
	end = isize >> blkbits;
	holeoff = offset;

	do {
		map.m_lblk = last;
		map.m_len = end - last + 1;
		ret = ext4_map_blocks(NULL, inode, &map, 0);
		if (ret > 0 && !(map.m_flags & EXT4_MAP_UNWRITTEN)) {
			last += ret;
			holeoff = (loff_t)last << blkbits;
			continue;
		}

		/*
		 * If there is a delay extent at this offset,
		 * we will skip this extent.
		 */
		ext4_es_find_delayed_extent_range(inode, last, last, &es);
		if (es.es_len != 0 && in_range(last, es.es_lblk, es.es_len)) {
			last = es.es_lblk + es.es_len;
			holeoff = (loff_t)last << blkbits;
			continue;
		}

		/*
		 * If there is a unwritten extent at this offset,
		 * it will be as a data or a hole according to page
		 * cache that has data or not.
		 */
		if (map.m_flags & EXT4_MAP_UNWRITTEN) {
			int unwritten;
			unwritten = ext4_find_unwritten_pgoff(inode, SEEK_HOLE,
							      &map, &holeoff);
			if (!unwritten) {
				last += ret;
				holeoff = (loff_t)last << blkbits;
				continue;
			}
		}

		/* find a hole */
		break;
	} while (last <= end);

	mutex_unlock(&inode->i_mutex);

	if (holeoff > isize)
		holeoff = isize;

	return vfs_setpos(file, holeoff, maxsize);
}
