static int vivid_fb_set_par(struct fb_info *info)
{
	int rc = 0;
	struct vivid_dev *dev = (struct vivid_dev *) info->par;

	dprintk(dev, 1, "vivid_fb_set_par\n");

	rc = vivid_fb_set_var(dev, &info->var);
	vivid_fb_get_fix(dev, &info->fix);
	return rc;
}
