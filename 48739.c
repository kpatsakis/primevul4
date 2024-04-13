bool_t auth_gssapi_seal_seq(
     gss_ctx_id_t context,
     uint32_t seq_num,
     gss_buffer_t out_buf)
{
     gss_buffer_desc in_buf;
     OM_uint32 gssstat, minor_stat;
     uint32_t nl_seq_num;

     nl_seq_num = htonl(seq_num);

     in_buf.length = sizeof(uint32_t);
     in_buf.value = (char *) &nl_seq_num;
     gssstat = gss_seal(&minor_stat, context, 0, GSS_C_QOP_DEFAULT,
			&in_buf, NULL, out_buf);
     if (gssstat != GSS_S_COMPLETE) {
	  PRINTF(("gssapi_seal_seq: failed\n"));
	  AUTH_GSSAPI_DISPLAY_STATUS(("sealing sequence number",
				      gssstat, minor_stat));
	  return FALSE;
     }
     return TRUE;
}
