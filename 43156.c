smb2_echo_callback(struct mid_q_entry *mid)
{
	struct TCP_Server_Info *server = mid->callback_data;
	struct smb2_echo_rsp *smb2 = (struct smb2_echo_rsp *)mid->resp_buf;
	unsigned int credits_received = 1;

	if (mid->mid_state == MID_RESPONSE_RECEIVED)
		credits_received = le16_to_cpu(smb2->hdr.CreditRequest);

	DeleteMidQEntry(mid);
	add_credits(server, credits_received, CIFS_ECHO_OP);
}
