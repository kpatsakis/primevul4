static void floppy_end_request(struct request *req, int error)
{
	unsigned int nr_sectors = current_count_sectors;
	unsigned int drive = (unsigned long)req->rq_disk->private_data;

	/* current_count_sectors can be zero if transfer failed */
	if (error)
		nr_sectors = blk_rq_cur_sectors(req);
	if (__blk_end_request(req, error, nr_sectors << 9))
		return;

	/* We're done with the request */
	floppy_off(drive);
	current_req = NULL;
}
