cifs_readpages_read_into_pages(struct TCP_Server_Info *server,
			struct cifs_readdata *rdata, unsigned int len)
{
	int total_read = 0, result = 0;
	unsigned int i;
	u64 eof;
	pgoff_t eof_index;
	unsigned int nr_pages = rdata->nr_pages;
	struct kvec iov;

	/* determine the eof that the server (probably) has */
	eof = CIFS_I(rdata->mapping->host)->server_eof;
	eof_index = eof ? (eof - 1) >> PAGE_CACHE_SHIFT : 0;
	cifs_dbg(FYI, "eof=%llu eof_index=%lu\n", eof, eof_index);

	rdata->tailsz = PAGE_CACHE_SIZE;
	for (i = 0; i < nr_pages; i++) {
		struct page *page = rdata->pages[i];

		if (len >= PAGE_CACHE_SIZE) {
			/* enough data to fill the page */
			iov.iov_base = kmap(page);
			iov.iov_len = PAGE_CACHE_SIZE;
			cifs_dbg(FYI, "%u: idx=%lu iov_base=%p iov_len=%zu\n",
				 i, page->index, iov.iov_base, iov.iov_len);
			len -= PAGE_CACHE_SIZE;
		} else if (len > 0) {
			/* enough for partial page, fill and zero the rest */
			iov.iov_base = kmap(page);
			iov.iov_len = len;
			cifs_dbg(FYI, "%u: idx=%lu iov_base=%p iov_len=%zu\n",
				 i, page->index, iov.iov_base, iov.iov_len);
			memset(iov.iov_base + len,
				'\0', PAGE_CACHE_SIZE - len);
			rdata->tailsz = len;
			len = 0;
		} else if (page->index > eof_index) {
			/*
			 * The VFS will not try to do readahead past the
			 * i_size, but it's possible that we have outstanding
			 * writes with gaps in the middle and the i_size hasn't
			 * caught up yet. Populate those with zeroed out pages
			 * to prevent the VFS from repeatedly attempting to
			 * fill them until the writes are flushed.
			 */
			zero_user(page, 0, PAGE_CACHE_SIZE);
			lru_cache_add_file(page);
			flush_dcache_page(page);
			SetPageUptodate(page);
			unlock_page(page);
			page_cache_release(page);
			rdata->pages[i] = NULL;
			rdata->nr_pages--;
			continue;
		} else {
			/* no need to hold page hostage */
			lru_cache_add_file(page);
			unlock_page(page);
			page_cache_release(page);
			rdata->pages[i] = NULL;
			rdata->nr_pages--;
			continue;
		}

		result = cifs_readv_from_socket(server, &iov, 1, iov.iov_len);
		kunmap(page);
		if (result < 0)
			break;

		total_read += result;
	}

	return total_read > 0 ? total_read : result;
}
