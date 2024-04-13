static int __readpage_endio_check(struct inode *inode,
				  struct btrfs_io_bio *io_bio,
				  int icsum, struct page *page,
				  int pgoff, u64 start, size_t len)
{
	char *kaddr;
	u32 csum_expected;
	u32 csum = ~(u32)0;

	csum_expected = *(((u32 *)io_bio->csum) + icsum);

	kaddr = kmap_atomic(page);
	csum = btrfs_csum_data(kaddr + pgoff, csum,  len);
	btrfs_csum_final(csum, (char *)&csum);
	if (csum != csum_expected)
		goto zeroit;

	kunmap_atomic(kaddr);
	return 0;
zeroit:
	btrfs_warn_rl(BTRFS_I(inode)->root->fs_info,
		"csum failed ino %llu off %llu csum %u expected csum %u",
			   btrfs_ino(inode), start, csum, csum_expected);
	memset(kaddr + pgoff, 1, len);
	flush_dcache_page(page);
	kunmap_atomic(kaddr);
	if (csum_expected == 0)
		return 0;
	return -EIO;
}
