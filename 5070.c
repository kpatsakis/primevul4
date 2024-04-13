void btrfs_commit_device_sizes(struct btrfs_transaction *trans)
{
	struct btrfs_device *curr, *next;

	ASSERT(trans->state == TRANS_STATE_COMMIT_DOING);

	if (list_empty(&trans->dev_update_list))
		return;

	/*
	 * We don't need the device_list_mutex here.  This list is owned by the
	 * transaction and the transaction must complete before the device is
	 * released.
	 */
	mutex_lock(&trans->fs_info->chunk_mutex);
	list_for_each_entry_safe(curr, next, &trans->dev_update_list,
				 post_commit_list) {
		list_del_init(&curr->post_commit_list);
		curr->commit_total_bytes = curr->disk_total_bytes;
		curr->commit_bytes_used = curr->bytes_used;
	}
	mutex_unlock(&trans->fs_info->chunk_mutex);
}