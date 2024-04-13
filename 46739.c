super_1_allow_new_offset(struct md_rdev *rdev,
			 unsigned long long new_offset)
{
	/* All necessary checks on new >= old have been done */
	struct bitmap *bitmap;
	if (new_offset >= rdev->data_offset)
		return 1;

	/* with 1.0 metadata, there is no metadata to tread on
	 * so we can always move back */
	if (rdev->mddev->minor_version == 0)
		return 1;

	/* otherwise we must be sure not to step on
	 * any metadata, so stay:
	 * 36K beyond start of superblock
	 * beyond end of badblocks
	 * beyond write-intent bitmap
	 */
	if (rdev->sb_start + (32+4)*2 > new_offset)
		return 0;
	bitmap = rdev->mddev->bitmap;
	if (bitmap && !rdev->mddev->bitmap_info.file &&
	    rdev->sb_start + rdev->mddev->bitmap_info.offset +
	    bitmap->storage.file_pages * (PAGE_SIZE>>9) > new_offset)
		return 0;
	if (rdev->badblocks.sector + rdev->badblocks.size > new_offset)
		return 0;

	return 1;
}
