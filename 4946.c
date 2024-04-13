void btrfs_mapping_tree_free(struct extent_map_tree *tree)
{
	struct extent_map *em;

	while (1) {
		write_lock(&tree->lock);
		em = lookup_extent_mapping(tree, 0, (u64)-1);
		if (em)
			remove_extent_mapping(tree, em);
		write_unlock(&tree->lock);
		if (!em)
			break;
		/* once for us */
		free_extent_map(em);
		/* once for the tree */
		free_extent_map(em);
	}
}