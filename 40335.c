static void fb_do_show_logo(struct fb_info *info, struct fb_image *image,
			    int rotate, unsigned int num)
{
	unsigned int x;

	if (rotate == FB_ROTATE_UR) {
		for (x = 0;
		     x < num && image->dx + image->width <= info->var.xres;
		     x++) {
			info->fbops->fb_imageblit(info, image);
			image->dx += image->width + 8;
		}
	} else if (rotate == FB_ROTATE_UD) {
		for (x = 0; x < num && image->dx >= 0; x++) {
			info->fbops->fb_imageblit(info, image);
			image->dx -= image->width + 8;
		}
	} else if (rotate == FB_ROTATE_CW) {
		for (x = 0;
		     x < num && image->dy + image->height <= info->var.yres;
		     x++) {
			info->fbops->fb_imageblit(info, image);
			image->dy += image->height + 8;
		}
	} else if (rotate == FB_ROTATE_CCW) {
		for (x = 0; x < num && image->dy >= 0; x++) {
			info->fbops->fb_imageblit(info, image);
			image->dy -= image->height + 8;
		}
	}
}
