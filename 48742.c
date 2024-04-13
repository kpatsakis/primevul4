bool_t xdr_authgssapi_init_arg(
     XDR *xdrs,
     auth_gssapi_init_arg *init_arg)
{
     if (! xdr_u_int32(xdrs, &init_arg->version) ||
	 ! xdr_gss_buf(xdrs, &init_arg->token))
	  return FALSE;
     return TRUE;
}
