bool_t xdr_authgssapi_init_res(
     XDR *xdrs,
     auth_gssapi_init_res *init_res)
{
     if (! xdr_u_int32(xdrs, &init_res->version) ||
	 ! xdr_gss_buf(xdrs, &init_res->client_handle) ||
	 ! xdr_u_int32(xdrs, &init_res->gss_major) ||
	 ! xdr_u_int32(xdrs, &init_res->gss_minor) ||
	 ! xdr_gss_buf(xdrs, &init_res->token) ||
	 ! xdr_gss_buf(xdrs, &init_res->signed_isn))
	  return FALSE;
     return TRUE;
}
