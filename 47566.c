static u8 get_x86_model(unsigned long sig)
{
	u8 x86, x86_model;

	x86 = get_x86_family(sig);
	x86_model = (sig >> 4) & 0xf;

	if (x86 == 0x6 || x86 == 0xf)
		x86_model += ((sig >> 16) & 0xf) << 4;

	return x86_model;
}
