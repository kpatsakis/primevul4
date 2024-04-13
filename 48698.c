xdr_chrand_ret(XDR *xdrs, chrand_ret *objp)
{
	if (!xdr_ui_4(xdrs, &objp->api_version)) {
		return (FALSE);
	}
	if (!xdr_kadm5_ret_t(xdrs, &objp->code)) {
		return (FALSE);
	}
	if (objp->code == KADM5_OK) {
		if (!xdr_array(xdrs, (char **)&objp->keys,
			       (unsigned int *)&objp->n_keys, ~0,
			       sizeof(krb5_keyblock), xdr_krb5_keyblock))
			return FALSE;
	}

	return (TRUE);
}
