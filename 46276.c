static int _vivid_fb_check_var(struct fb_var_screeninfo *var, struct vivid_dev *dev)
{
	dprintk(dev, 1, "vivid_fb_check_var\n");

	var->bits_per_pixel = 16;
	if (var->green.length == 5) {
		var->red.offset = 10;
		var->red.length = 5;
		var->green.offset = 5;
		var->green.length = 5;
		var->blue.offset = 0;
		var->blue.length = 5;
		var->transp.offset = 15;
		var->transp.length = 1;
	} else {
		var->red.offset = 11;
		var->red.length = 5;
		var->green.offset = 5;
		var->green.length = 6;
		var->blue.offset = 0;
		var->blue.length = 5;
		var->transp.offset = 0;
		var->transp.length = 0;
	}
	var->xoffset = var->yoffset = 0;
	var->left_margin = var->upper_margin = 0;
	var->nonstd = 0;

	var->vmode &= ~FB_VMODE_MASK;
	var->vmode = FB_VMODE_NONINTERLACED;

	/* Dummy values */
	var->hsync_len = 24;
	var->vsync_len = 2;
	var->pixclock = 84316;
	var->right_margin = 776;
	var->lower_margin = 591;
	return 0;
}
