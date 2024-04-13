static int vivid_fb_init_vidmode(struct vivid_dev *dev)
{
	struct v4l2_rect start_window;

	/* Color mode */

	dev->bits_per_pixel = 16;
	dev->bytes_per_pixel = dev->bits_per_pixel / 8;

	start_window.width = MAX_OSD_WIDTH;
	start_window.left = 0;

	dev->display_byte_stride = start_window.width * dev->bytes_per_pixel;

	/* Vertical size & position */

	start_window.height = MAX_OSD_HEIGHT;
	start_window.top = 0;

	dev->display_width = start_window.width;
	dev->display_height = start_window.height;

	/* Generate a valid fb_var_screeninfo */

	dev->fb_defined.xres = dev->display_width;
	dev->fb_defined.yres = dev->display_height;
	dev->fb_defined.xres_virtual = dev->display_width;
	dev->fb_defined.yres_virtual = dev->display_height;
	dev->fb_defined.bits_per_pixel = dev->bits_per_pixel;
	dev->fb_defined.vmode = FB_VMODE_NONINTERLACED;
	dev->fb_defined.left_margin = start_window.left + 1;
	dev->fb_defined.upper_margin = start_window.top + 1;
	dev->fb_defined.accel_flags = FB_ACCEL_NONE;
	dev->fb_defined.nonstd = 0;
	/* set default to 1:5:5:5 */
	dev->fb_defined.green.length = 5;

	/* We've filled in the most data, let the usual mode check
	   routine fill in the rest. */
	_vivid_fb_check_var(&dev->fb_defined, dev);

	/* Generate valid fb_fix_screeninfo */

	vivid_fb_get_fix(dev, &dev->fb_fix);

	/* Generate valid fb_info */

	dev->fb_info.node = -1;
	dev->fb_info.flags = FBINFO_FLAG_DEFAULT;
	dev->fb_info.fbops = &vivid_fb_ops;
	dev->fb_info.par = dev;
	dev->fb_info.var = dev->fb_defined;
	dev->fb_info.fix = dev->fb_fix;
	dev->fb_info.screen_base = (u8 __iomem *)dev->video_vbase;
	dev->fb_info.fbops = &vivid_fb_ops;

	/* Supply some monitor specs. Bogus values will do for now */
	dev->fb_info.monspecs.hfmin = 8000;
	dev->fb_info.monspecs.hfmax = 70000;
	dev->fb_info.monspecs.vfmin = 10;
	dev->fb_info.monspecs.vfmax = 100;

	/* Allocate color map */
	if (fb_alloc_cmap(&dev->fb_info.cmap, 256, 1)) {
		pr_err("abort, unable to alloc cmap\n");
		return -ENOMEM;
	}

	/* Allocate the pseudo palette */
	dev->fb_info.pseudo_palette = kmalloc_array(16, sizeof(u32), GFP_KERNEL);

	return dev->fb_info.pseudo_palette ? 0 : -ENOMEM;
}
