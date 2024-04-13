xfs_find_get_desired_pgoff(
	struct inode		*inode,
	struct xfs_bmbt_irec	*map,
	unsigned int		type,
	loff_t			*offset)
{
	struct xfs_inode	*ip = XFS_I(inode);
	struct xfs_mount	*mp = ip->i_mount;
	struct pagevec		pvec;
	pgoff_t			index;
	pgoff_t			end;
	loff_t			endoff;
	loff_t			startoff = *offset;
	loff_t			lastoff = startoff;
	bool			found = false;

	pagevec_init(&pvec, 0);

	index = startoff >> PAGE_CACHE_SHIFT;
	endoff = XFS_FSB_TO_B(mp, map->br_startoff + map->br_blockcount);
	end = endoff >> PAGE_CACHE_SHIFT;
	do {
		int		want;
		unsigned	nr_pages;
		unsigned int	i;

		want = min_t(pgoff_t, end - index, PAGEVEC_SIZE);
		nr_pages = pagevec_lookup(&pvec, inode->i_mapping, index,
					  want);
		/*
		 * No page mapped into given range.  If we are searching holes
		 * and if this is the first time we got into the loop, it means
		 * that the given offset is landed in a hole, return it.
		 *
		 * If we have already stepped through some block buffers to find
		 * holes but they all contains data.  In this case, the last
		 * offset is already updated and pointed to the end of the last
		 * mapped page, if it does not reach the endpoint to search,
		 * that means there should be a hole between them.
		 */
		if (nr_pages == 0) {
			/* Data search found nothing */
			if (type == DATA_OFF)
				break;

			ASSERT(type == HOLE_OFF);
			if (lastoff == startoff || lastoff < endoff) {
				found = true;
				*offset = lastoff;
			}
			break;
		}

		/*
		 * At lease we found one page.  If this is the first time we
		 * step into the loop, and if the first page index offset is
		 * greater than the given search offset, a hole was found.
		 */
		if (type == HOLE_OFF && lastoff == startoff &&
		    lastoff < page_offset(pvec.pages[0])) {
			found = true;
			break;
		}

		for (i = 0; i < nr_pages; i++) {
			struct page	*page = pvec.pages[i];
			loff_t		b_offset;

			/*
			 * At this point, the page may be truncated or
			 * invalidated (changing page->mapping to NULL),
			 * or even swizzled back from swapper_space to tmpfs
			 * file mapping. However, page->index will not change
			 * because we have a reference on the page.
			 *
			 * Searching done if the page index is out of range.
			 * If the current offset is not reaches the end of
			 * the specified search range, there should be a hole
			 * between them.
			 */
			if (page->index > end) {
				if (type == HOLE_OFF && lastoff < endoff) {
					*offset = lastoff;
					found = true;
				}
				goto out;
			}

			lock_page(page);
			/*
			 * Page truncated or invalidated(page->mapping == NULL).
			 * We can freely skip it and proceed to check the next
			 * page.
			 */
			if (unlikely(page->mapping != inode->i_mapping)) {
				unlock_page(page);
				continue;
			}

			if (!page_has_buffers(page)) {
				unlock_page(page);
				continue;
			}

			found = xfs_lookup_buffer_offset(page, &b_offset, type);
			if (found) {
				/*
				 * The found offset may be less than the start
				 * point to search if this is the first time to
				 * come here.
				 */
				*offset = max_t(loff_t, startoff, b_offset);
				unlock_page(page);
				goto out;
			}

			/*
			 * We either searching data but nothing was found, or
			 * searching hole but found a data buffer.  In either
			 * case, probably the next page contains the desired
			 * things, update the last offset to it so.
			 */
			lastoff = page_offset(page) + PAGE_SIZE;
			unlock_page(page);
		}

		/*
		 * The number of returned pages less than our desired, search
		 * done.  In this case, nothing was found for searching data,
		 * but we found a hole behind the last offset.
		 */
		if (nr_pages < want) {
			if (type == HOLE_OFF) {
				*offset = lastoff;
				found = true;
			}
			break;
		}

		index = pvec.pages[i - 1]->index + 1;
		pagevec_release(&pvec);
	} while (index <= end);

out:
	pagevec_release(&pvec);
	return found;
}
