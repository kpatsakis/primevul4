static int vivid_fb_get_fix(struct vivid_dev *dev, struct fb_fix_screeninfo *fix)
{
	dprintk(dev, 1, "vivid_fb_get_fix\n");
	memset(fix, 0, sizeof(struct fb_fix_screeninfo));
	strlcpy(fix->id, "vioverlay fb", sizeof(fix->id));
	fix->smem_start = dev->video_pbase;
	fix->smem_len = dev->video_buffer_size;
	fix->type = FB_TYPE_PACKED_PIXELS;
	fix->visual = FB_VISUAL_TRUECOLOR;
	fix->xpanstep = 1;
	fix->ypanstep = 1;
	fix->ywrapstep = 0;
	fix->line_length = dev->display_byte_stride;
	fix->accel = FB_ACCEL_NONE;
	return 0;
}
