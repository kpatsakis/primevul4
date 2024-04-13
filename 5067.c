static int chunk_soft_convert_filter(u64 chunk_type,
				     struct btrfs_balance_args *bargs)
{
	if (!(bargs->flags & BTRFS_BALANCE_ARGS_CONVERT))
		return 0;

	chunk_type = chunk_to_extended(chunk_type) &
				BTRFS_EXTENDED_PROFILE_MASK;

	if (bargs->target == chunk_type)
		return 1;

	return 0;
}