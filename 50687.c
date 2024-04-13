static int packet_rcv_has_room(struct packet_sock *po, struct sk_buff *skb)
{
	int ret;
	bool has_room;

	spin_lock_bh(&po->sk.sk_receive_queue.lock);
	ret = __packet_rcv_has_room(po, skb);
	has_room = ret == ROOM_NORMAL;
	if (po->pressure == has_room)
		po->pressure = !has_room;
	spin_unlock_bh(&po->sk.sk_receive_queue.lock);

	return ret;
}
