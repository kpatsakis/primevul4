int lock_fb_info(struct fb_info *info)
{
	mutex_lock(&info->lock);
	if (!info->fbops) {
		mutex_unlock(&info->lock);
		return 0;
	}
	return 1;
}
