static int vivid_fb_set_var(struct vivid_dev *dev, struct fb_var_screeninfo *var)
{
	dprintk(dev, 1, "vivid_fb_set_var\n");

	if (var->bits_per_pixel != 16) {
		dprintk(dev, 1, "vivid_fb_set_var - Invalid bpp\n");
		return -EINVAL;
	}
	dev->display_byte_stride = var->xres * dev->bytes_per_pixel;

	return 0;
}
