static int fb_show_logo_line(struct fb_info *info, int rotate,
			     const struct linux_logo *logo, int y,
			     unsigned int n)
{
	u32 *palette = NULL, *saved_pseudo_palette = NULL;
	unsigned char *logo_new = NULL, *logo_rotate = NULL;
	struct fb_image image;

	/* Return if the frame buffer is not mapped or suspended */
	if (logo == NULL || info->state != FBINFO_STATE_RUNNING ||
	    info->flags & FBINFO_MODULE)
		return 0;

	image.depth = 8;
	image.data = logo->data;

	if (fb_logo.needs_cmapreset)
		fb_set_logocmap(info, logo);

	if (fb_logo.needs_truepalette ||
	    fb_logo.needs_directpalette) {
		palette = kmalloc(256 * 4, GFP_KERNEL);
		if (palette == NULL)
			return 0;

		if (fb_logo.needs_truepalette)
			fb_set_logo_truepalette(info, logo, palette);
		else
			fb_set_logo_directpalette(info, logo, palette);

		saved_pseudo_palette = info->pseudo_palette;
		info->pseudo_palette = palette;
	}

	if (fb_logo.depth <= 4) {
		logo_new = kmalloc(logo->width * logo->height, GFP_KERNEL);
		if (logo_new == NULL) {
			kfree(palette);
			if (saved_pseudo_palette)
				info->pseudo_palette = saved_pseudo_palette;
			return 0;
		}
		image.data = logo_new;
		fb_set_logo(info, logo, logo_new, fb_logo.depth);
	}

	image.dx = 0;
	image.dy = y;
	image.width = logo->width;
	image.height = logo->height;

	if (rotate) {
		logo_rotate = kmalloc(logo->width *
				      logo->height, GFP_KERNEL);
		if (logo_rotate)
			fb_rotate_logo(info, logo_rotate, &image, rotate);
	}

	fb_do_show_logo(info, &image, rotate, n);

	kfree(palette);
	if (saved_pseudo_palette != NULL)
		info->pseudo_palette = saved_pseudo_palette;
	kfree(logo_new);
	kfree(logo_rotate);
	return logo->height;
}
