static void ims_pcu_handle_response(struct ims_pcu *pcu)
{
	switch (pcu->read_buf[0]) {
	case IMS_PCU_RSP_EVNT_BUTTONS:
		if (likely(pcu->setup_complete))
			ims_pcu_report_events(pcu);
		break;

	default:
		/*
		 * See if we got command completion.
		 * If both the sequence and response code match save
		 * the data and signal completion.
		 */
		if (pcu->read_buf[0] == pcu->expected_response &&
		    pcu->read_buf[1] == pcu->ack_id - 1) {

			memcpy(pcu->cmd_buf, pcu->read_buf, pcu->read_pos);
			pcu->cmd_buf_len = pcu->read_pos;
			complete(&pcu->cmd_done);
		}
		break;
	}
}
