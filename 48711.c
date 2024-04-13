xdr_gprincs_arg(XDR *xdrs, gprincs_arg *objp)
{
     if (!xdr_ui_4(xdrs, &objp->api_version)) {
	  return (FALSE);
     }
     if (!xdr_nullstring(xdrs, &objp->exp)) {
	  return (FALSE);
     }
     return (TRUE);
}
