static void btrfs_set_dev_stats_value(struct extent_buffer *eb,
				      struct btrfs_dev_stats_item *ptr,
				      int index, u64 val)
{
	write_extent_buffer(eb, &val,
			    offsetof(struct btrfs_dev_stats_item, values) +
			     ((unsigned long)ptr) + (index * sizeof(u64)),
			    sizeof(val));
}