bool btrfs_pinned_by_swapfile(struct btrfs_fs_info *fs_info, void *ptr)
{
	struct btrfs_swapfile_pin *sp;
	struct rb_node *node;

	spin_lock(&fs_info->swapfile_pins_lock);
	node = fs_info->swapfile_pins.rb_node;
	while (node) {
		sp = rb_entry(node, struct btrfs_swapfile_pin, node);
		if (ptr < sp->ptr)
			node = node->rb_left;
		else if (ptr > sp->ptr)
			node = node->rb_right;
		else
			break;
	}
	spin_unlock(&fs_info->swapfile_pins_lock);
	return node != NULL;
}