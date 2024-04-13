static void cm_reject_sidr_req(struct cm_id_private *cm_id_priv,
			       enum ib_cm_sidr_status status)
{
	struct ib_cm_sidr_rep_param param;

	memset(&param, 0, sizeof param);
	param.status = status;
	ib_send_cm_sidr_rep(&cm_id_priv->id, &param);
}
