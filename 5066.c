static u64 btrfs_dev_stats_value(const struct extent_buffer *eb,
				 const struct btrfs_dev_stats_item *ptr,
				 int index)
{
	u64 val;

	read_extent_buffer(eb, &val,
			   offsetof(struct btrfs_dev_stats_item, values) +
			    ((unsigned long)ptr) + (index * sizeof(u64)),
			   sizeof(val));
	return val;
}