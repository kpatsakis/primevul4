static void cm_format_sidr_req(struct cm_sidr_req_msg *sidr_req_msg,
			       struct cm_id_private *cm_id_priv,
			       struct ib_cm_sidr_req_param *param)
{
	cm_format_mad_hdr(&sidr_req_msg->hdr, CM_SIDR_REQ_ATTR_ID,
			  cm_form_tid(cm_id_priv, CM_MSG_SEQUENCE_SIDR));
	sidr_req_msg->request_id = cm_id_priv->id.local_id;
	sidr_req_msg->pkey = param->path->pkey;
	sidr_req_msg->service_id = param->service_id;

	if (param->private_data && param->private_data_len)
		memcpy(sidr_req_msg->private_data, param->private_data,
		       param->private_data_len);
}
