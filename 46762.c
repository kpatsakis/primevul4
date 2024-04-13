static ssize_t ubb_show(struct md_rdev *rdev, char *page)
{
	return badblocks_show(&rdev->badblocks, page, 1);
}
