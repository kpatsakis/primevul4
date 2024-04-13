int security_net_peersid_resolve(u32 nlbl_sid, u32 nlbl_type,
				 u32 xfrm_sid,
				 u32 *peer_sid)
{
	int rc;
	struct context *nlbl_ctx;
	struct context *xfrm_ctx;

	*peer_sid = SECSID_NULL;

	/* handle the common (which also happens to be the set of easy) cases
	 * right away, these two if statements catch everything involving a
	 * single or absent peer SID/label */
	if (xfrm_sid == SECSID_NULL) {
		*peer_sid = nlbl_sid;
		return 0;
	}
	/* NOTE: an nlbl_type == NETLBL_NLTYPE_UNLABELED is a "fallback" label
	 * and is treated as if nlbl_sid == SECSID_NULL when a XFRM SID/label
	 * is present */
	if (nlbl_sid == SECSID_NULL || nlbl_type == NETLBL_NLTYPE_UNLABELED) {
		*peer_sid = xfrm_sid;
		return 0;
	}

	/* we don't need to check ss_initialized here since the only way both
	 * nlbl_sid and xfrm_sid are not equal to SECSID_NULL would be if the
	 * security server was initialized and ss_initialized was true */
	if (!policydb.mls_enabled)
		return 0;

	read_lock(&policy_rwlock);

	rc = -EINVAL;
	nlbl_ctx = sidtab_search(&sidtab, nlbl_sid);
	if (!nlbl_ctx) {
		printk(KERN_ERR "SELinux: %s:  unrecognized SID %d\n",
		       __func__, nlbl_sid);
		goto out;
	}
	rc = -EINVAL;
	xfrm_ctx = sidtab_search(&sidtab, xfrm_sid);
	if (!xfrm_ctx) {
		printk(KERN_ERR "SELinux: %s:  unrecognized SID %d\n",
		       __func__, xfrm_sid);
		goto out;
	}
	rc = (mls_context_cmp(nlbl_ctx, xfrm_ctx) ? 0 : -EACCES);
	if (rc)
		goto out;

	/* at present NetLabel SIDs/labels really only carry MLS
	 * information so if the MLS portion of the NetLabel SID
	 * matches the MLS portion of the labeled XFRM SID/label
	 * then pass along the XFRM SID as it is the most
	 * expressive */
	*peer_sid = xfrm_sid;
out:
	read_unlock(&policy_rwlock);
	return rc;
}
