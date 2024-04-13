static void fb_rotate_logo_ud(const u8 *in, u8 *out, u32 width, u32 height)
{
	u32 size = width * height, i;

	out += size - 1;

	for (i = size; i--; )
		*out-- = *in++;
}
