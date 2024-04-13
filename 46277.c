void vivid_clear_fb(struct vivid_dev *dev)
{
	void *p = dev->video_vbase;
	const u16 *rgb = rgb555;
	unsigned x, y;

	if (dev->fb_defined.green.length == 6)
		rgb = rgb565;

	for (y = 0; y < dev->display_height; y++) {
		u16 *d = p;

		for (x = 0; x < dev->display_width; x++)
			d[x] = rgb[(y / 16 + x / 16) % 16];
		p += dev->display_byte_stride;
	}
}
