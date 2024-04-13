struct bio *bio_copy_user_iov(struct request_queue *q,
			      struct rq_map_data *map_data,
			      const struct iov_iter *iter,
			      gfp_t gfp_mask)
{
	struct bio_map_data *bmd;
	struct page *page;
	struct bio *bio;
	int i, ret;
	int nr_pages = 0;
	unsigned int len = iter->count;
	unsigned int offset = map_data ? offset_in_page(map_data->offset) : 0;

	for (i = 0; i < iter->nr_segs; i++) {
		unsigned long uaddr;
		unsigned long end;
		unsigned long start;

		uaddr = (unsigned long) iter->iov[i].iov_base;
		end = (uaddr + iter->iov[i].iov_len + PAGE_SIZE - 1)
			>> PAGE_SHIFT;
		start = uaddr >> PAGE_SHIFT;

		/*
		 * Overflow, abort
		 */
		if (end < start)
			return ERR_PTR(-EINVAL);

		nr_pages += end - start;
	}

	if (offset)
		nr_pages++;

	bmd = bio_alloc_map_data(iter->nr_segs, gfp_mask);
	if (!bmd)
		return ERR_PTR(-ENOMEM);

	/*
	 * We need to do a deep copy of the iov_iter including the iovecs.
	 * The caller provided iov might point to an on-stack or otherwise
	 * shortlived one.
	 */
	bmd->is_our_pages = map_data ? 0 : 1;
	memcpy(bmd->iov, iter->iov, sizeof(struct iovec) * iter->nr_segs);
	iov_iter_init(&bmd->iter, iter->type, bmd->iov,
			iter->nr_segs, iter->count);

	ret = -ENOMEM;
	bio = bio_kmalloc(gfp_mask, nr_pages);
	if (!bio)
		goto out_bmd;

	ret = 0;

	if (map_data) {
		nr_pages = 1 << map_data->page_order;
		i = map_data->offset / PAGE_SIZE;
	}
	while (len) {
		unsigned int bytes = PAGE_SIZE;

		bytes -= offset;

		if (bytes > len)
			bytes = len;

		if (map_data) {
			if (i == map_data->nr_entries * nr_pages) {
				ret = -ENOMEM;
				break;
			}

			page = map_data->pages[i / nr_pages];
			page += (i % nr_pages);

			i++;
		} else {
			page = alloc_page(q->bounce_gfp | gfp_mask);
			if (!page) {
				ret = -ENOMEM;
				break;
			}
		}

		if (bio_add_pc_page(q, bio, page, bytes, offset) < bytes)
			break;

		len -= bytes;
		offset = 0;
	}

	if (ret)
		goto cleanup;

	/*
	 * success
	 */
	if (((iter->type & WRITE) && (!map_data || !map_data->null_mapped)) ||
	    (map_data && map_data->from_user)) {
		ret = bio_copy_from_iter(bio, *iter);
		if (ret)
			goto cleanup;
	}

	bio->bi_private = bmd;
	return bio;
cleanup:
	if (!map_data)
		bio_free_pages(bio);
	bio_put(bio);
out_bmd:
	kfree(bmd);
	return ERR_PTR(ret);
}
