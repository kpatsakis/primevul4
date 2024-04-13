static int buffer_chain_size(void)
{
	struct bio_vec bv;
	int size;
	struct req_iterator iter;
	char *base;

	base = bio_data(current_req->bio);
	size = 0;

	rq_for_each_segment(bv, current_req, iter) {
		if (page_address(bv.bv_page) + bv.bv_offset != base + size)
			break;

		size += bv.bv_len;
	}

	return size >> 9;
}
