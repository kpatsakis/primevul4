void btrfs_release_disk_super(struct btrfs_super_block *super)
{
	struct page *page = virt_to_page(super);

	put_page(page);
}