static void do_fd_request(struct request_queue *q)
{
	if (WARN(max_buffer_sectors == 0,
		 "VFS: %s called on non-open device\n", __func__))
		return;

	if (WARN(atomic_read(&usage_count) == 0,
		 "warning: usage count=0, current_req=%p sect=%ld type=%x flags=%llx\n",
		 current_req, (long)blk_rq_pos(current_req), current_req->cmd_type,
		 (unsigned long long) current_req->cmd_flags))
		return;

	if (test_and_set_bit(0, &fdc_busy)) {
		/* fdc busy, this new request will be treated when the
		   current one is done */
		is_alive(__func__, "old request running");
		return;
	}
	command_status = FD_COMMAND_NONE;
	__reschedule_timeout(MAXTIMEOUT, "fd_request");
	set_fdc(0);
	process_fd_request();
	is_alive(__func__, "");
}
