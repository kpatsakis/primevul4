unregister_framebuffer(struct fb_info *fb_info)
{
	int ret;

	mutex_lock(&registration_lock);
	ret = do_unregister_framebuffer(fb_info);
	mutex_unlock(&registration_lock);

	return ret;
}
