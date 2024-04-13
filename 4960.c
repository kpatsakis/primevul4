static void readahead_tree_node_children(struct extent_buffer *node)
{
	int i;
	const int nr_items = btrfs_header_nritems(node);

	for (i = 0; i < nr_items; i++)
		btrfs_readahead_node_child(node, i);
}