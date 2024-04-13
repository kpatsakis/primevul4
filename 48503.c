static inline unsigned int gcm_remain(unsigned int len)
{
	len &= 0xfU;
	return len ? 16 - len : 0;
}
