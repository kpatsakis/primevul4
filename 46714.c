reshape_direction_show(struct mddev *mddev, char *page)
{
	return sprintf(page, "%s\n",
		       mddev->reshape_backwards ? "backwards" : "forwards");
}
