static int ext4_find_unwritten_pgoff(struct inode *inode,
				     int whence,
				     struct ext4_map_blocks *map,
				     loff_t *offset)
{
	struct pagevec pvec;
	unsigned int blkbits;
	pgoff_t index;
	pgoff_t end;
	loff_t endoff;
	loff_t startoff;
	loff_t lastoff;
	int found = 0;

	blkbits = inode->i_sb->s_blocksize_bits;
	startoff = *offset;
	lastoff = startoff;
	endoff = (loff_t)(map->m_lblk + map->m_len) << blkbits;

	index = startoff >> PAGE_CACHE_SHIFT;
	end = endoff >> PAGE_CACHE_SHIFT;

	pagevec_init(&pvec, 0);
	do {
		int i, num;
		unsigned long nr_pages;

		num = min_t(pgoff_t, end - index, PAGEVEC_SIZE);
		nr_pages = pagevec_lookup(&pvec, inode->i_mapping, index,
					  (pgoff_t)num);
		if (nr_pages == 0) {
			if (whence == SEEK_DATA)
				break;

			BUG_ON(whence != SEEK_HOLE);
			/*
			 * If this is the first time to go into the loop and
			 * offset is not beyond the end offset, it will be a
			 * hole at this offset
			 */
			if (lastoff == startoff || lastoff < endoff)
				found = 1;
			break;
		}

		/*
		 * If this is the first time to go into the loop and
		 * offset is smaller than the first page offset, it will be a
		 * hole at this offset.
		 */
		if (lastoff == startoff && whence == SEEK_HOLE &&
		    lastoff < page_offset(pvec.pages[0])) {
			found = 1;
			break;
		}

		for (i = 0; i < nr_pages; i++) {
			struct page *page = pvec.pages[i];
			struct buffer_head *bh, *head;

			/*
			 * If the current offset is not beyond the end of given
			 * range, it will be a hole.
			 */
			if (lastoff < endoff && whence == SEEK_HOLE &&
			    page->index > end) {
				found = 1;
				*offset = lastoff;
				goto out;
			}

			lock_page(page);

			if (unlikely(page->mapping != inode->i_mapping)) {
				unlock_page(page);
				continue;
			}

			if (!page_has_buffers(page)) {
				unlock_page(page);
				continue;
			}

			if (page_has_buffers(page)) {
				lastoff = page_offset(page);
				bh = head = page_buffers(page);
				do {
					if (buffer_uptodate(bh) ||
					    buffer_unwritten(bh)) {
						if (whence == SEEK_DATA)
							found = 1;
					} else {
						if (whence == SEEK_HOLE)
							found = 1;
					}
					if (found) {
						*offset = max_t(loff_t,
							startoff, lastoff);
						unlock_page(page);
						goto out;
					}
					lastoff += bh->b_size;
					bh = bh->b_this_page;
				} while (bh != head);
			}

			lastoff = page_offset(page) + PAGE_SIZE;
			unlock_page(page);
		}

		/*
		 * The no. of pages is less than our desired, that would be a
		 * hole in there.
		 */
		if (nr_pages < num && whence == SEEK_HOLE) {
			found = 1;
			*offset = lastoff;
			break;
		}

		index = pvec.pages[i - 1]->index + 1;
		pagevec_release(&pvec);
	} while (index <= end);

out:
	pagevec_release(&pvec);
	return found;
}
