static void fb_rotate_logo_cw(const u8 *in, u8 *out, u32 width, u32 height)
{
	int i, j, h = height - 1;

	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
				out[height * j + h - i] = *in++;
}
