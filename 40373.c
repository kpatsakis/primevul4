int unlink_framebuffer(struct fb_info *fb_info)
{
	int i;

	i = fb_info->node;
	if (i < 0 || i >= FB_MAX || registered_fb[i] != fb_info)
		return -EINVAL;

	if (fb_info->dev) {
		device_destroy(fb_class, MKDEV(FB_MAJOR, i));
		fb_info->dev = NULL;
	}
	return 0;
}
