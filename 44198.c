static void cm_format_req(struct cm_req_msg *req_msg,
			  struct cm_id_private *cm_id_priv,
			  struct ib_cm_req_param *param)
{
	struct ib_sa_path_rec *pri_path = param->primary_path;
	struct ib_sa_path_rec *alt_path = param->alternate_path;

	cm_format_mad_hdr(&req_msg->hdr, CM_REQ_ATTR_ID,
			  cm_form_tid(cm_id_priv, CM_MSG_SEQUENCE_REQ));

	req_msg->local_comm_id = cm_id_priv->id.local_id;
	req_msg->service_id = param->service_id;
	req_msg->local_ca_guid = cm_id_priv->id.device->node_guid;
	cm_req_set_local_qpn(req_msg, cpu_to_be32(param->qp_num));
	cm_req_set_init_depth(req_msg, param->initiator_depth);
	cm_req_set_remote_resp_timeout(req_msg,
				       param->remote_cm_response_timeout);
	cm_req_set_qp_type(req_msg, param->qp_type);
	cm_req_set_flow_ctrl(req_msg, param->flow_control);
	cm_req_set_starting_psn(req_msg, cpu_to_be32(param->starting_psn));
	cm_req_set_local_resp_timeout(req_msg,
				      param->local_cm_response_timeout);
	req_msg->pkey = param->primary_path->pkey;
	cm_req_set_path_mtu(req_msg, param->primary_path->mtu);
	cm_req_set_max_cm_retries(req_msg, param->max_cm_retries);

	if (param->qp_type != IB_QPT_XRC_INI) {
		cm_req_set_resp_res(req_msg, param->responder_resources);
		cm_req_set_retry_count(req_msg, param->retry_count);
		cm_req_set_rnr_retry_count(req_msg, param->rnr_retry_count);
		cm_req_set_srq(req_msg, param->srq);
	}

	if (pri_path->hop_limit <= 1) {
		req_msg->primary_local_lid = pri_path->slid;
		req_msg->primary_remote_lid = pri_path->dlid;
	} else {
		/* Work-around until there's a way to obtain remote LID info */
		req_msg->primary_local_lid = IB_LID_PERMISSIVE;
		req_msg->primary_remote_lid = IB_LID_PERMISSIVE;
	}
	req_msg->primary_local_gid = pri_path->sgid;
	req_msg->primary_remote_gid = pri_path->dgid;
	cm_req_set_primary_flow_label(req_msg, pri_path->flow_label);
	cm_req_set_primary_packet_rate(req_msg, pri_path->rate);
	req_msg->primary_traffic_class = pri_path->traffic_class;
	req_msg->primary_hop_limit = pri_path->hop_limit;
	cm_req_set_primary_sl(req_msg, pri_path->sl);
	cm_req_set_primary_subnet_local(req_msg, (pri_path->hop_limit <= 1));
	cm_req_set_primary_local_ack_timeout(req_msg,
		cm_ack_timeout(cm_id_priv->av.port->cm_dev->ack_delay,
			       pri_path->packet_life_time));

	if (alt_path) {
		if (alt_path->hop_limit <= 1) {
			req_msg->alt_local_lid = alt_path->slid;
			req_msg->alt_remote_lid = alt_path->dlid;
		} else {
			req_msg->alt_local_lid = IB_LID_PERMISSIVE;
			req_msg->alt_remote_lid = IB_LID_PERMISSIVE;
		}
		req_msg->alt_local_gid = alt_path->sgid;
		req_msg->alt_remote_gid = alt_path->dgid;
		cm_req_set_alt_flow_label(req_msg,
					  alt_path->flow_label);
		cm_req_set_alt_packet_rate(req_msg, alt_path->rate);
		req_msg->alt_traffic_class = alt_path->traffic_class;
		req_msg->alt_hop_limit = alt_path->hop_limit;
		cm_req_set_alt_sl(req_msg, alt_path->sl);
		cm_req_set_alt_subnet_local(req_msg, (alt_path->hop_limit <= 1));
		cm_req_set_alt_local_ack_timeout(req_msg,
			cm_ack_timeout(cm_id_priv->av.port->cm_dev->ack_delay,
				       alt_path->packet_life_time));
	}

	if (param->private_data && param->private_data_len)
		memcpy(req_msg->private_data, param->private_data,
		       param->private_data_len);
}
