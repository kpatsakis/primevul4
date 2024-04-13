cifs_uncached_read_into_pages(struct TCP_Server_Info *server,
			struct cifs_readdata *rdata, unsigned int len)
{
	int total_read = 0, result = 0;
	unsigned int i;
	unsigned int nr_pages = rdata->nr_pages;
	struct kvec iov;

	rdata->tailsz = PAGE_SIZE;
	for (i = 0; i < nr_pages; i++) {
		struct page *page = rdata->pages[i];

		if (len >= PAGE_SIZE) {
			/* enough data to fill the page */
			iov.iov_base = kmap(page);
			iov.iov_len = PAGE_SIZE;
			cifs_dbg(FYI, "%u: iov_base=%p iov_len=%zu\n",
				 i, iov.iov_base, iov.iov_len);
			len -= PAGE_SIZE;
		} else if (len > 0) {
			/* enough for partial page, fill and zero the rest */
			iov.iov_base = kmap(page);
			iov.iov_len = len;
			cifs_dbg(FYI, "%u: iov_base=%p iov_len=%zu\n",
				 i, iov.iov_base, iov.iov_len);
			memset(iov.iov_base + len, '\0', PAGE_SIZE - len);
			rdata->tailsz = len;
			len = 0;
		} else {
			/* no need to hold page hostage */
			rdata->pages[i] = NULL;
			rdata->nr_pages--;
			put_page(page);
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
