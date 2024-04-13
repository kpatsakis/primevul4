static void copy_buffer(int ssize, int max_sector, int max_sector_2)
{
	int remaining;		/* number of transferred 512-byte sectors */
	struct bio_vec bv;
	char *buffer;
	char *dma_buffer;
	int size;
	struct req_iterator iter;

	max_sector = transfer_size(ssize,
				   min(max_sector, max_sector_2),
				   blk_rq_sectors(current_req));

	if (current_count_sectors <= 0 && CT(COMMAND) == FD_WRITE &&
	    buffer_max > fsector_t + blk_rq_sectors(current_req))
		current_count_sectors = min_t(int, buffer_max - fsector_t,
					      blk_rq_sectors(current_req));

	remaining = current_count_sectors << 9;
	if (remaining > blk_rq_bytes(current_req) && CT(COMMAND) == FD_WRITE) {
		DPRINT("in copy buffer\n");
		pr_info("current_count_sectors=%ld\n", current_count_sectors);
		pr_info("remaining=%d\n", remaining >> 9);
		pr_info("current_req->nr_sectors=%u\n",
			blk_rq_sectors(current_req));
		pr_info("current_req->current_nr_sectors=%u\n",
			blk_rq_cur_sectors(current_req));
		pr_info("max_sector=%d\n", max_sector);
		pr_info("ssize=%d\n", ssize);
	}

	buffer_max = max(max_sector, buffer_max);

	dma_buffer = floppy_track_buffer + ((fsector_t - buffer_min) << 9);

	size = blk_rq_cur_bytes(current_req);

	rq_for_each_segment(bv, current_req, iter) {
		if (!remaining)
			break;

		size = bv.bv_len;
		SUPBOUND(size, remaining);

		buffer = page_address(bv.bv_page) + bv.bv_offset;
		if (dma_buffer + size >
		    floppy_track_buffer + (max_buffer_sectors << 10) ||
		    dma_buffer < floppy_track_buffer) {
			DPRINT("buffer overrun in copy buffer %d\n",
			       (int)((floppy_track_buffer - dma_buffer) >> 9));
			pr_info("fsector_t=%d buffer_min=%d\n",
				fsector_t, buffer_min);
			pr_info("current_count_sectors=%ld\n",
				current_count_sectors);
			if (CT(COMMAND) == FD_READ)
				pr_info("read\n");
			if (CT(COMMAND) == FD_WRITE)
				pr_info("write\n");
			break;
		}
		if (((unsigned long)buffer) % 512)
			DPRINT("%p buffer not aligned\n", buffer);

		if (CT(COMMAND) == FD_READ)
			memcpy(buffer, dma_buffer, size);
		else
			memcpy(dma_buffer, buffer, size);

		remaining -= size;
		dma_buffer += size;
	}
	if (remaining) {
		if (remaining > 0)
			max_sector -= remaining >> 9;
		DPRINT("weirdness: remaining %d\n", remaining >> 9);
	}
}
