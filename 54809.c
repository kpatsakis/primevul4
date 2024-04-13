static bool uas_evaluate_response_iu(struct response_iu *riu, struct scsi_cmnd *cmnd)
{
	u8 response_code = riu->response_code;

	switch (response_code) {
	case RC_INCORRECT_LUN:
		cmnd->result = DID_BAD_TARGET << 16;
		break;
	case RC_TMF_SUCCEEDED:
		cmnd->result = DID_OK << 16;
		break;
	case RC_TMF_NOT_SUPPORTED:
		cmnd->result = DID_TARGET_FAILURE << 16;
		break;
	default:
		uas_log_cmd_state(cmnd, "response iu", response_code);
		cmnd->result = DID_ERROR << 16;
		break;
	}

	return response_code == RC_TMF_SUCCEEDED;
}
