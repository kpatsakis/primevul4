static int chunk_profiles_filter(u64 chunk_type,
				 struct btrfs_balance_args *bargs)
{
	chunk_type = chunk_to_extended(chunk_type) &
				BTRFS_EXTENDED_PROFILE_MASK;

	if (bargs->profiles & chunk_type)
		return 0;

	return 1;
}