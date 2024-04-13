static void cm_format_rej_event(struct cm_work *work)
{
	struct cm_rej_msg *rej_msg;
	struct ib_cm_rej_event_param *param;

	rej_msg = (struct cm_rej_msg *)work->mad_recv_wc->recv_buf.mad;
	param = &work->cm_event.param.rej_rcvd;
	param->ari = rej_msg->ari;
	param->ari_length = cm_rej_get_reject_info_len(rej_msg);
	param->reason = __be16_to_cpu(rej_msg->reason);
	work->cm_event.private_data = &rej_msg->private_data;
}
