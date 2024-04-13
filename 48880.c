__generic_file_splice_read(struct file *in, loff_t *ppos,
			   struct pipe_inode_info *pipe, size_t len,
			   unsigned int flags)
{
	struct address_space *mapping = in->f_mapping;
	unsigned int loff, nr_pages, req_pages;
	struct page *pages[PIPE_DEF_BUFFERS];
	struct partial_page partial[PIPE_DEF_BUFFERS];
	struct page *page;
	pgoff_t index, end_index;
	loff_t isize;
	int error, page_nr;
	struct splice_pipe_desc spd = {
		.pages = pages,
		.partial = partial,
		.nr_pages_max = PIPE_DEF_BUFFERS,
		.flags = flags,
		.ops = &page_cache_pipe_buf_ops,
		.spd_release = spd_release_page,
	};

	if (splice_grow_spd(pipe, &spd))
		return -ENOMEM;

	index = *ppos >> PAGE_CACHE_SHIFT;
	loff = *ppos & ~PAGE_CACHE_MASK;
	req_pages = (len + loff + PAGE_CACHE_SIZE - 1) >> PAGE_CACHE_SHIFT;
	nr_pages = min(req_pages, spd.nr_pages_max);

	/*
	 * Lookup the (hopefully) full range of pages we need.
	 */
	spd.nr_pages = find_get_pages_contig(mapping, index, nr_pages, spd.pages);
	index += spd.nr_pages;

	/*
	 * If find_get_pages_contig() returned fewer pages than we needed,
	 * readahead/allocate the rest and fill in the holes.
	 */
	if (spd.nr_pages < nr_pages)
		page_cache_sync_readahead(mapping, &in->f_ra, in,
				index, req_pages - spd.nr_pages);

	error = 0;
	while (spd.nr_pages < nr_pages) {
		/*
		 * Page could be there, find_get_pages_contig() breaks on
		 * the first hole.
		 */
		page = find_get_page(mapping, index);
		if (!page) {
			/*
			 * page didn't exist, allocate one.
			 */
			page = page_cache_alloc_cold(mapping);
			if (!page)
				break;

			error = add_to_page_cache_lru(page, mapping, index,
						GFP_KERNEL);
			if (unlikely(error)) {
				page_cache_release(page);
				if (error == -EEXIST)
					continue;
				break;
			}
			/*
			 * add_to_page_cache() locks the page, unlock it
			 * to avoid convoluting the logic below even more.
			 */
			unlock_page(page);
		}

		spd.pages[spd.nr_pages++] = page;
		index++;
	}

	/*
	 * Now loop over the map and see if we need to start IO on any
	 * pages, fill in the partial map, etc.
	 */
	index = *ppos >> PAGE_CACHE_SHIFT;
	nr_pages = spd.nr_pages;
	spd.nr_pages = 0;
	for (page_nr = 0; page_nr < nr_pages; page_nr++) {
		unsigned int this_len;

		if (!len)
			break;

		/*
		 * this_len is the max we'll use from this page
		 */
		this_len = min_t(unsigned long, len, PAGE_CACHE_SIZE - loff);
		page = spd.pages[page_nr];

		if (PageReadahead(page))
			page_cache_async_readahead(mapping, &in->f_ra, in,
					page, index, req_pages - page_nr);

		/*
		 * If the page isn't uptodate, we may need to start io on it
		 */
		if (!PageUptodate(page)) {
			lock_page(page);

			/*
			 * Page was truncated, or invalidated by the
			 * filesystem.  Redo the find/create, but this time the
			 * page is kept locked, so there's no chance of another
			 * race with truncate/invalidate.
			 */
			if (!page->mapping) {
				unlock_page(page);
				page = find_or_create_page(mapping, index,
						mapping_gfp_mask(mapping));

				if (!page) {
					error = -ENOMEM;
					break;
				}
				page_cache_release(spd.pages[page_nr]);
				spd.pages[page_nr] = page;
			}
			/*
			 * page was already under io and is now done, great
			 */
			if (PageUptodate(page)) {
				unlock_page(page);
				goto fill_it;
			}

			/*
			 * need to read in the page
			 */
			error = mapping->a_ops->readpage(in, page);
			if (unlikely(error)) {
				/*
				 * We really should re-lookup the page here,
				 * but it complicates things a lot. Instead
				 * lets just do what we already stored, and
				 * we'll get it the next time we are called.
				 */
				if (error == AOP_TRUNCATED_PAGE)
					error = 0;

				break;
			}
		}
fill_it:
		/*
		 * i_size must be checked after PageUptodate.
		 */
		isize = i_size_read(mapping->host);
		end_index = (isize - 1) >> PAGE_CACHE_SHIFT;
		if (unlikely(!isize || index > end_index))
			break;

		/*
		 * if this is the last page, see if we need to shrink
		 * the length and stop
		 */
		if (end_index == index) {
			unsigned int plen;

			/*
			 * max good bytes in this page
			 */
			plen = ((isize - 1) & ~PAGE_CACHE_MASK) + 1;
			if (plen <= loff)
				break;

			/*
			 * force quit after adding this page
			 */
			this_len = min(this_len, plen - loff);
			len = this_len;
		}

		spd.partial[page_nr].offset = loff;
		spd.partial[page_nr].len = this_len;
		len -= this_len;
		loff = 0;
		spd.nr_pages++;
		index++;
	}

	/*
	 * Release any pages at the end, if we quit early. 'page_nr' is how far
	 * we got, 'nr_pages' is how many pages are in the map.
	 */
	while (page_nr < nr_pages)
		page_cache_release(spd.pages[page_nr++]);
	in->f_ra.prev_pos = (loff_t)index << PAGE_CACHE_SHIFT;

	if (spd.nr_pages)
		error = splice_to_pipe(pipe, &spd);

	splice_shrink_spd(&spd);
	return error;
}
