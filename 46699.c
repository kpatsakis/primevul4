offset_show(struct md_rdev *rdev, char *page)
{
	return sprintf(page, "%llu\n", (unsigned long long)rdev->data_offset);
}
