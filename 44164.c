static int be64_lt(__be64 a, __be64 b)
{
	return (__force u64) a < (__force u64) b;
}
