static s32 snto32(__u32 value, unsigned n)
{
	switch (n) {
	case 8:  return ((__s8)value);
	case 16: return ((__s16)value);
	case 32: return ((__s32)value);
	}
	return value & (1 << (n - 1)) ? value | (-1 << n) : value;
}
