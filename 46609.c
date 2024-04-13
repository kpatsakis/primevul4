level_show(struct mddev *mddev, char *page)
{
	struct md_personality *p;
	int ret;
	spin_lock(&mddev->lock);
	p = mddev->pers;
	if (p)
		ret = sprintf(page, "%s\n", p->name);
	else if (mddev->clevel[0])
		ret = sprintf(page, "%s\n", mddev->clevel);
	else if (mddev->level != LEVEL_NONE)
		ret = sprintf(page, "%d\n", mddev->level);
	else
		ret = 0;
	spin_unlock(&mddev->lock);
	return ret;
}
