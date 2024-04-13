static int decide_stripe_size_regular(struct alloc_chunk_ctl *ctl,
				      struct btrfs_device_info *devices_info)
{
	/* Number of stripes that count for block group size */
	int data_stripes;

	/*
	 * The primary goal is to maximize the number of stripes, so use as
	 * many devices as possible, even if the stripes are not maximum sized.
	 *
	 * The DUP profile stores more than one stripe per device, the
	 * max_avail is the total size so we have to adjust.
	 */
	ctl->stripe_size = div_u64(devices_info[ctl->ndevs - 1].max_avail,
				   ctl->dev_stripes);
	ctl->num_stripes = ctl->ndevs * ctl->dev_stripes;

	/* This will have to be fixed for RAID1 and RAID10 over more drives */
	data_stripes = (ctl->num_stripes - ctl->nparity) / ctl->ncopies;

	/*
	 * Use the number of data stripes to figure out how big this chunk is
	 * really going to be in terms of logical address space, and compare
	 * that answer with the max chunk size. If it's higher, we try to
	 * reduce stripe_size.
	 */
	if (ctl->stripe_size * data_stripes > ctl->max_chunk_size) {
		/*
		 * Reduce stripe_size, round it up to a 16MB boundary again and
		 * then use it, unless it ends up being even bigger than the
		 * previous value we had already.
		 */
		ctl->stripe_size = min(round_up(div_u64(ctl->max_chunk_size,
							data_stripes), SZ_16M),
				       ctl->stripe_size);
	}

	/* Align to BTRFS_STRIPE_LEN */
	ctl->stripe_size = round_down(ctl->stripe_size, BTRFS_STRIPE_LEN);
	ctl->chunk_size = ctl->stripe_size * data_stripes;

	return 0;
}