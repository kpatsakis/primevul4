static int set_next_request(void)
{
	struct request_queue *q;
	int old_pos = fdc_queue;

	do {
		q = disks[fdc_queue]->queue;
		if (++fdc_queue == N_DRIVE)
			fdc_queue = 0;
		if (q) {
			current_req = blk_fetch_request(q);
			if (current_req)
				break;
		}
	} while (fdc_queue != old_pos);

	return current_req != NULL;
}
