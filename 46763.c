static ssize_t ubb_store(struct md_rdev *rdev, const char *page, size_t len)
{
	return badblocks_store(&rdev->badblocks, page, len, 1);
}
