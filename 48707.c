xdr_gpols_arg(XDR *xdrs, gpols_arg *objp)
{
     if (!xdr_ui_4(xdrs, &objp->api_version)) {
	  return (FALSE);
     }
     if (!xdr_nullstring(xdrs, &objp->exp)) {
	  return (FALSE);
     }
     return (TRUE);
}
