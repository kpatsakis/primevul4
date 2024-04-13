bool_t xdr_authgssapi_creds(
     XDR *xdrs,
     auth_gssapi_creds *creds)
{
     if (! xdr_u_int32(xdrs, &creds->version) ||
	 ! xdr_bool(xdrs, &creds->auth_msg) ||
	 ! xdr_gss_buf(xdrs, &creds->client_handle))
       return FALSE;
     return TRUE;
}
