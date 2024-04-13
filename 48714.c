bool_t xdr_kadm5_principal_ent_rec(XDR *xdrs,
				   kadm5_principal_ent_rec *objp)
{
     return _xdr_kadm5_principal_ent_rec(xdrs, objp, KADM5_API_VERSION_3);
}
