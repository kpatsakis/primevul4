static void cm_free_msg(struct ib_mad_send_buf *msg)
{
	ib_destroy_ah(msg->ah);
	if (msg->context[0])
		cm_deref_id(msg->context[0]);
	ib_free_send_mad(msg);
}
