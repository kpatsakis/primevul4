_xdr_kadm5_policy_ent_rec(XDR *xdrs, kadm5_policy_ent_rec *objp, int vers)
{
	if (!xdr_nullstring(xdrs, &objp->policy)) {
		return (FALSE);
	}
	/* these all used to be u_int32, but it's stupid for sized types
	   to be exposed at the api, and they're the same as longs on the
	   wire. */
	if (!xdr_long(xdrs, &objp->pw_min_life)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->pw_max_life)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->pw_min_length)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->pw_min_classes)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->pw_history_num)) {
		return (FALSE);
	}
	if (!xdr_long(xdrs, &objp->policy_refcnt)) {
		return (FALSE);
	}
	if (xdrs->x_op == XDR_DECODE) {
		objp->pw_max_fail = 0;
		objp->pw_failcnt_interval = 0;
		objp->pw_lockout_duration = 0;
		objp->attributes = 0;
		objp->max_life = 0;
		objp->max_renewable_life = 0;
		objp->allowed_keysalts = NULL;
		objp->n_tl_data = 0;
		objp->tl_data = NULL;
	}
	if (vers >= KADM5_API_VERSION_3) {
		if (!xdr_krb5_kvno(xdrs, &objp->pw_max_fail))
			return (FALSE);
		if (!xdr_krb5_deltat(xdrs, &objp->pw_failcnt_interval))
			return (FALSE);
		if (!xdr_krb5_deltat(xdrs, &objp->pw_lockout_duration))
			return (FALSE);
	}
	if (vers >= KADM5_API_VERSION_4) {
		if (!xdr_krb5_flags(xdrs, &objp->attributes)) {
			return (FALSE);
		}
		if (!xdr_krb5_deltat(xdrs, &objp->max_life)) {
			return (FALSE);
		}
		if (!xdr_krb5_deltat(xdrs, &objp->max_renewable_life)) {
			return (FALSE);
		}
		if (!xdr_nullstring(xdrs, &objp->allowed_keysalts)) {
			return (FALSE);
		}
		if (!xdr_krb5_int16(xdrs, &objp->n_tl_data)) {
			return (FALSE);
		}
		if (!xdr_nulltype(xdrs, (void **) &objp->tl_data,
				  xdr_krb5_tl_data)) {
			return FALSE;
		}
	}
	return (TRUE);
}
