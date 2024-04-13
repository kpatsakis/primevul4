static bool __reg64_bound_u32(u64 a)
{
	return a >= U32_MIN && a <= U32_MAX;
}