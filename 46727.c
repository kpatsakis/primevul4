size_show(struct mddev *mddev, char *page)
{
	return sprintf(page, "%llu\n",
		(unsigned long long)mddev->dev_sectors / 2);
}
