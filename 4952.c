void btrfs_free_device(struct btrfs_device *device)
{
	WARN_ON(!list_empty(&device->post_commit_list));
	rcu_string_free(device->name);
	extent_io_tree_release(&device->alloc_state);
	bio_put(device->flush_bio);
	btrfs_destroy_dev_zone_info(device);
	kfree(device);
}