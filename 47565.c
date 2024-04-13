static u8 get_x86_family(unsigned long sig)
{
	u8 x86;

	x86 = (sig >> 8) & 0xf;

	if (x86 == 0xf)
		x86 += (sig >> 20) & 0xff;

	return x86;
}
