static void btrfs_split_extent_hook(struct inode *inode,
				    struct extent_state *orig, u64 split)
{
	u64 size;

	/* not delalloc, ignore it */
	if (!(orig->state & EXTENT_DELALLOC))
		return;

	size = orig->end - orig->start + 1;
	if (size > BTRFS_MAX_EXTENT_SIZE) {
		u64 num_extents;
		u64 new_size;

		/*
		 * See the explanation in btrfs_merge_extent_hook, the same
		 * applies here, just in reverse.
		 */
		new_size = orig->end - split + 1;
		num_extents = div64_u64(new_size + BTRFS_MAX_EXTENT_SIZE - 1,
					BTRFS_MAX_EXTENT_SIZE);
		new_size = split - orig->start;
		num_extents += div64_u64(new_size + BTRFS_MAX_EXTENT_SIZE - 1,
					BTRFS_MAX_EXTENT_SIZE);
		if (div64_u64(size + BTRFS_MAX_EXTENT_SIZE - 1,
			      BTRFS_MAX_EXTENT_SIZE) >= num_extents)
			return;
	}

	spin_lock(&BTRFS_I(inode)->lock);
	BTRFS_I(inode)->outstanding_extents++;
	spin_unlock(&BTRFS_I(inode)->lock);
}
