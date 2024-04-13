static bool __reg64_bound_s32(s64 a)
{
	return a >= S32_MIN && a <= S32_MAX;
}