super_90_allow_new_offset(struct md_rdev *rdev, unsigned long long new_offset)
{
	/* non-zero offset changes not possible with v0.90 */
	return new_offset == 0;
}
