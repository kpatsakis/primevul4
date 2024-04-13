static int vivid_fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	struct vivid_dev *dev = (struct vivid_dev *) info->par;

	dprintk(dev, 1, "vivid_fb_check_var\n");
	return _vivid_fb_check_var(var, dev);
}
