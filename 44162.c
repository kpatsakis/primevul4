static int be32_gt(__be32 a, __be32 b)
{
	return (__force u32) a > (__force u32) b;
}
