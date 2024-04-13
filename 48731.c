bool_t xdr_nulltype(XDR *xdrs, void **objp, xdrproc_t proc)
{
     bool_t null;

     switch (xdrs->x_op) {
     case XDR_DECODE:
	  if (!xdr_bool(xdrs, &null))
	      return FALSE;
	  if (null) {
	       *objp = NULL;
	       return TRUE;
	  }
	  return (*proc)(xdrs, objp);

     case XDR_ENCODE:
	  if (*objp == NULL)
	       null = TRUE;
	  else
	       null = FALSE;
	  if (!xdr_bool(xdrs, &null))
	       return FALSE;
	  if (null == FALSE)
	       return (*proc)(xdrs, objp);
	  return TRUE;

     case XDR_FREE:
	  if (*objp)
	       return (*proc)(xdrs, objp);
	  return TRUE;
     }

     return FALSE;
}
