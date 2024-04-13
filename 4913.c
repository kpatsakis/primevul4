static int chunk_stripes_range_filter(struct extent_buffer *leaf,
			       struct btrfs_chunk *chunk,
			       struct btrfs_balance_args *bargs)
{
	int num_stripes = btrfs_chunk_num_stripes(leaf, chunk);

	if (bargs->stripes_min <= num_stripes
			&& num_stripes <= bargs->stripes_max)
		return 0;

	return 1;
}