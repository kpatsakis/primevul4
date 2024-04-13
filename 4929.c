static int decide_stripe_size_zoned(struct alloc_chunk_ctl *ctl,
				    struct btrfs_device_info *devices_info)
{
	u64 zone_size = devices_info[0].dev->zone_info->zone_size;
	/* Number of stripes that count for block group size */
	int data_stripes;

	/*
	 * It should hold because:
	 *    dev_extent_min == dev_extent_want == zone_size * dev_stripes
	 */
	ASSERT(devices_info[ctl->ndevs - 1].max_avail == ctl->dev_extent_min);

	ctl->stripe_size = zone_size;
	ctl->num_stripes = ctl->ndevs * ctl->dev_stripes;
	data_stripes = (ctl->num_stripes - ctl->nparity) / ctl->ncopies;

	/* stripe_size is fixed in zoned filesysmte. Reduce ndevs instead. */
	if (ctl->stripe_size * data_stripes > ctl->max_chunk_size) {
		ctl->ndevs = div_u64(div_u64(ctl->max_chunk_size * ctl->ncopies,
					     ctl->stripe_size) + ctl->nparity,
				     ctl->dev_stripes);
		ctl->num_stripes = ctl->ndevs * ctl->dev_stripes;
		data_stripes = (ctl->num_stripes - ctl->nparity) / ctl->ncopies;
		ASSERT(ctl->stripe_size * data_stripes <= ctl->max_chunk_size);
	}

	ctl->chunk_size = ctl->stripe_size * data_stripes;

	return 0;
}