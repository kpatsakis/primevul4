int md_rdev_init(struct md_rdev *rdev)
{
	rdev->desc_nr = -1;
	rdev->saved_raid_disk = -1;
	rdev->raid_disk = -1;
	rdev->flags = 0;
	rdev->data_offset = 0;
	rdev->new_data_offset = 0;
	rdev->sb_events = 0;
	rdev->last_read_error.tv_sec  = 0;
	rdev->last_read_error.tv_nsec = 0;
	rdev->sb_loaded = 0;
	rdev->bb_page = NULL;
	atomic_set(&rdev->nr_pending, 0);
	atomic_set(&rdev->read_errors, 0);
	atomic_set(&rdev->corrected_errors, 0);

	INIT_LIST_HEAD(&rdev->same_set);
	init_waitqueue_head(&rdev->blocked_wait);

	/* Add space to store bad block list.
	 * This reserves the space even on arrays where it cannot
	 * be used - I wonder if that matters
	 */
	rdev->badblocks.count = 0;
	rdev->badblocks.shift = -1; /* disabled until explicitly enabled */
	rdev->badblocks.page = kmalloc(PAGE_SIZE, GFP_KERNEL);
	seqlock_init(&rdev->badblocks.lock);
	if (rdev->badblocks.page == NULL)
		return -ENOMEM;

	return 0;
}
