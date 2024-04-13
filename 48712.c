xdr_gprincs_ret(XDR *xdrs, gprincs_ret *objp)
{
     if (!xdr_ui_4(xdrs, &objp->api_version)) {
	  return (FALSE);
     }
     if (!xdr_kadm5_ret_t(xdrs, &objp->code)) {
	  return (FALSE);
     }
     if (objp->code == KADM5_OK) {
	  if (!xdr_int(xdrs, &objp->count)) {
	       return (FALSE);
	  }
	  if (!xdr_array(xdrs, (caddr_t *) &objp->princs,
			 (unsigned int *) &objp->count, ~0,
			 sizeof(char *), xdr_nullstring)) {
	       return (FALSE);
	  }
     }

     return (TRUE);
}
