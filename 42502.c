static void bat_socket_add_packet(struct socket_client *socket_client,
				  struct icmp_packet_rr *icmp_packet,
				  size_t icmp_len)
{
	struct socket_packet *socket_packet;

	socket_packet = kmalloc(sizeof(*socket_packet), GFP_ATOMIC);

	if (!socket_packet)
		return;

	INIT_LIST_HEAD(&socket_packet->list);
	memcpy(&socket_packet->icmp_packet, icmp_packet, icmp_len);
	socket_packet->icmp_len = icmp_len;

	spin_lock_bh(&socket_client->lock);

	/* while waiting for the lock the socket_client could have been
	 * deleted */
	if (!socket_client_hash[icmp_packet->uid]) {
		spin_unlock_bh(&socket_client->lock);
		kfree(socket_packet);
		return;
	}

	list_add_tail(&socket_packet->list, &socket_client->queue_list);
	socket_client->queue_len++;

	if (socket_client->queue_len > 100) {
		socket_packet = list_first_entry(&socket_client->queue_list,
						 struct socket_packet, list);

		list_del(&socket_packet->list);
		kfree(socket_packet);
		socket_client->queue_len--;
	}

	spin_unlock_bh(&socket_client->lock);

	wake_up(&socket_client->queue_wait);
}
