static void btrfs_merge_extent_hook(struct inode *inode,
				    struct extent_state *new,
				    struct extent_state *other)
{
	u64 new_size, old_size;
	u64 num_extents;

	/* not delalloc, ignore it */
	if (!(other->state & EXTENT_DELALLOC))
		return;

	if (new->start > other->start)
		new_size = new->end - other->start + 1;
	else
		new_size = other->end - new->start + 1;

	/* we're not bigger than the max, unreserve the space and go */
	if (new_size <= BTRFS_MAX_EXTENT_SIZE) {
		spin_lock(&BTRFS_I(inode)->lock);
		BTRFS_I(inode)->outstanding_extents--;
		spin_unlock(&BTRFS_I(inode)->lock);
		return;
	}

	/*
	 * We have to add up either side to figure out how many extents were
	 * accounted for before we merged into one big extent.  If the number of
	 * extents we accounted for is <= the amount we need for the new range
	 * then we can return, otherwise drop.  Think of it like this
	 *
	 * [ 4k][MAX_SIZE]
	 *
	 * So we've grown the extent by a MAX_SIZE extent, this would mean we
	 * need 2 outstanding extents, on one side we have 1 and the other side
	 * we have 1 so they are == and we can return.  But in this case
	 *
	 * [MAX_SIZE+4k][MAX_SIZE+4k]
	 *
	 * Each range on their own accounts for 2 extents, but merged together
	 * they are only 3 extents worth of accounting, so we need to drop in
	 * this case.
	 */
	old_size = other->end - other->start + 1;
	num_extents = div64_u64(old_size + BTRFS_MAX_EXTENT_SIZE - 1,
				BTRFS_MAX_EXTENT_SIZE);
	old_size = new->end - new->start + 1;
	num_extents += div64_u64(old_size + BTRFS_MAX_EXTENT_SIZE - 1,
				 BTRFS_MAX_EXTENT_SIZE);

	if (div64_u64(new_size + BTRFS_MAX_EXTENT_SIZE - 1,
		      BTRFS_MAX_EXTENT_SIZE) >= num_extents)
		return;

	spin_lock(&BTRFS_I(inode)->lock);
	BTRFS_I(inode)->outstanding_extents--;
	spin_unlock(&BTRFS_I(inode)->lock);
}
