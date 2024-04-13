int rdev_clear_badblocks(struct md_rdev *rdev, sector_t s, int sectors,
			 int is_new)
{
	if (is_new)
		s += rdev->new_data_offset;
	else
		s += rdev->data_offset;
	return md_clear_badblocks(&rdev->badblocks,
				  s, sectors);
}
