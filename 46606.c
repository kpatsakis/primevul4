last_sync_action_show(struct mddev *mddev, char *page)
{
	return sprintf(page, "%s\n", mddev->last_sync_action);
}
