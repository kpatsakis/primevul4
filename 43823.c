static inline u64 rsvd_bits(int s, int e)
{
	return ((1ULL << (e - s + 1)) - 1) << s;
}
