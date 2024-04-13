static void cm_process_routed_req(struct cm_req_msg *req_msg, struct ib_wc *wc)
{
	if (!cm_req_get_primary_subnet_local(req_msg)) {
		if (req_msg->primary_local_lid == IB_LID_PERMISSIVE) {
			req_msg->primary_local_lid = cpu_to_be16(wc->slid);
			cm_req_set_primary_sl(req_msg, wc->sl);
		}

		if (req_msg->primary_remote_lid == IB_LID_PERMISSIVE)
			req_msg->primary_remote_lid = cpu_to_be16(wc->dlid_path_bits);
	}

	if (!cm_req_get_alt_subnet_local(req_msg)) {
		if (req_msg->alt_local_lid == IB_LID_PERMISSIVE) {
			req_msg->alt_local_lid = cpu_to_be16(wc->slid);
			cm_req_set_alt_sl(req_msg, wc->sl);
		}

		if (req_msg->alt_remote_lid == IB_LID_PERMISSIVE)
			req_msg->alt_remote_lid = cpu_to_be16(wc->dlid_path_bits);
	}
}
