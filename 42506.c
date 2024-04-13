static int bat_socket_release(struct inode *inode, struct file *file)
{
	struct socket_client *socket_client = file->private_data;
	struct socket_packet *socket_packet;
	struct list_head *list_pos, *list_pos_tmp;

	spin_lock_bh(&socket_client->lock);

	/* for all packets in the queue ... */
	list_for_each_safe(list_pos, list_pos_tmp, &socket_client->queue_list) {
		socket_packet = list_entry(list_pos,
					   struct socket_packet, list);

		list_del(list_pos);
		kfree(socket_packet);
	}

	socket_client_hash[socket_client->index] = NULL;
	spin_unlock_bh(&socket_client->lock);

	kfree(socket_client);
	dec_module_count();

	return 0;
}
