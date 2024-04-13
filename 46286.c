static int vivid_fb_setcolreg(unsigned regno, unsigned red, unsigned green,
				unsigned blue, unsigned transp,
				struct fb_info *info)
{
	u32 color, *palette;

	if (regno >= info->cmap.len)
		return -EINVAL;

	color = ((transp & 0xFF00) << 16) | ((red & 0xFF00) << 8) |
		 (green & 0xFF00) | ((blue & 0xFF00) >> 8);
	if (regno >= 16)
		return -EINVAL;

	palette = info->pseudo_palette;
	if (info->var.bits_per_pixel == 16) {
		switch (info->var.green.length) {
		case 6:
			color = (red & 0xf800) |
				((green & 0xfc00) >> 5) |
				((blue & 0xf800) >> 11);
			break;
		case 5:
			color = ((red & 0xf800) >> 1) |
				((green & 0xf800) >> 6) |
				((blue & 0xf800) >> 11) |
				(transp ? 0x8000 : 0);
			break;
		}
	}
	palette[regno] = color;
	return 0;
}
