static loff_t ext4_seek_data(struct file *file, loff_t offset, loff_t maxsize)
{
	struct inode *inode = file->f_mapping->host;
	struct ext4_map_blocks map;
	struct extent_status es;
	ext4_lblk_t start, last, end;
	loff_t dataoff, isize;
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
	dataoff = offset;

	do {
		map.m_lblk = last;
		map.m_len = end - last + 1;
		ret = ext4_map_blocks(NULL, inode, &map, 0);
		if (ret > 0 && !(map.m_flags & EXT4_MAP_UNWRITTEN)) {
			if (last != start)
				dataoff = (loff_t)last << blkbits;
			break;
		}

		/*
		 * If there is a delay extent at this offset,
		 * it will be as a data.
		 */
		ext4_es_find_delayed_extent_range(inode, last, last, &es);
		if (es.es_len != 0 && in_range(last, es.es_lblk, es.es_len)) {
			if (last != start)
				dataoff = (loff_t)last << blkbits;
			break;
		}

		/*
		 * If there is a unwritten extent at this offset,
		 * it will be as a data or a hole according to page
		 * cache that has data or not.
		 */
		if (map.m_flags & EXT4_MAP_UNWRITTEN) {
			int unwritten;
			unwritten = ext4_find_unwritten_pgoff(inode, SEEK_DATA,
							      &map, &dataoff);
			if (unwritten)
				break;
		}

		last++;
		dataoff = (loff_t)last << blkbits;
	} while (last <= end);

	mutex_unlock(&inode->i_mutex);

	if (dataoff > isize)
		return -ENXIO;

	return vfs_setpos(file, dataoff, maxsize);
}
