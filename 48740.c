bool_t auth_gssapi_wrap_data(
     OM_uint32 *major,
     OM_uint32 *minor,
     gss_ctx_id_t context,
     uint32_t seq_num,
     XDR *out_xdrs,
     bool_t (*xdr_func)(),
     caddr_t xdr_ptr)
{
     gss_buffer_desc in_buf, out_buf;
     XDR temp_xdrs;
     int conf_state;
     unsigned int length;

     PRINTF(("gssapi_wrap_data: starting\n"));

     *major = GSS_S_COMPLETE;
     *minor = 0; /* assumption */

     xdralloc_create(&temp_xdrs, XDR_ENCODE);

     /* serialize the sequence number into local memory */
     PRINTF(("gssapi_wrap_data: encoding seq_num %d\n", seq_num));
     if (! xdr_u_int32(&temp_xdrs, &seq_num)) {
	  PRINTF(("gssapi_wrap_data: serializing seq_num failed\n"));
	  XDR_DESTROY(&temp_xdrs);
	  return FALSE;
     }

     /* serialize the arguments into local memory */
     if (!(*xdr_func)(&temp_xdrs, xdr_ptr)) {
	  PRINTF(("gssapi_wrap_data: serializing arguments failed\n"));
	  XDR_DESTROY(&temp_xdrs);
	  return FALSE;
     }

     in_buf.length = xdr_getpos(&temp_xdrs);
     in_buf.value = xdralloc_getdata(&temp_xdrs);

     *major = gss_seal(minor, context, 1,
		       GSS_C_QOP_DEFAULT, &in_buf, &conf_state,
		       &out_buf);
     if (*major != GSS_S_COMPLETE) {
	  XDR_DESTROY(&temp_xdrs);
	  return FALSE;
     }

     PRINTF(("gssapi_wrap_data: %d bytes data, %d bytes sealed\n",
	     (int) in_buf.length, (int) out_buf.length));

     /* write the token */
     length = out_buf.length;
     if (! xdr_bytes(out_xdrs, (char **) &out_buf.value,
		     (unsigned int *) &length,
		     out_buf.length)) {
	  PRINTF(("gssapi_wrap_data: serializing encrypted data failed\n"));
	  XDR_DESTROY(&temp_xdrs);
	  return FALSE;
     }

     *major = gss_release_buffer(minor, &out_buf);

     PRINTF(("gssapi_wrap_data: succeeding\n\n"));
     XDR_DESTROY(&temp_xdrs);
     return TRUE;
}
