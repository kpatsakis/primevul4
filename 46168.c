static struct extent_map *btrfs_new_extent_direct(struct inode *inode,
						  u64 start, u64 len)
{
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct extent_map *em;
	struct btrfs_key ins;
	u64 alloc_hint;
	int ret;

	alloc_hint = get_extent_allocation_hint(inode, start, len);
	ret = btrfs_reserve_extent(root, len, root->sectorsize, 0,
				   alloc_hint, &ins, 1, 1);
	if (ret)
		return ERR_PTR(ret);

	em = create_pinned_em(inode, start, ins.offset, start, ins.objectid,
			      ins.offset, ins.offset, ins.offset, 0);
	if (IS_ERR(em)) {
		btrfs_free_reserved_extent(root, ins.objectid, ins.offset, 1);
		return em;
	}

	ret = btrfs_add_ordered_extent_dio(inode, start, ins.objectid,
					   ins.offset, ins.offset, 0);
	if (ret) {
		btrfs_free_reserved_extent(root, ins.objectid, ins.offset, 1);
		free_extent_map(em);
		return ERR_PTR(ret);
	}

	return em;
}
