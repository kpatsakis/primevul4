static void cm_format_sidr_rep_event(struct cm_work *work)
{
	struct cm_sidr_rep_msg *sidr_rep_msg;
	struct ib_cm_sidr_rep_event_param *param;

	sidr_rep_msg = (struct cm_sidr_rep_msg *)
				work->mad_recv_wc->recv_buf.mad;
	param = &work->cm_event.param.sidr_rep_rcvd;
	param->status = sidr_rep_msg->status;
	param->qkey = be32_to_cpu(sidr_rep_msg->qkey);
	param->qpn = be32_to_cpu(cm_sidr_rep_get_qpn(sidr_rep_msg));
	param->info = &sidr_rep_msg->info;
	param->info_len = sidr_rep_msg->info_length;
	work->cm_event.private_data = &sidr_rep_msg->private_data;
}
