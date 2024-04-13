devzvol_create(struct vnode *dvp, char *nm, struct vattr *vap, vcexcl_t excl,
    int mode, struct vnode **vpp, struct cred *cred, int flag,
    caller_context_t *ct, vsecattr_t *vsecp)
{
	int error;
	struct vnode *vp;

	*vpp = NULL;

	error = devzvol_lookup(dvp, nm, &vp, NULL, 0, NULL, cred, ct, NULL,
	    NULL);
	if (error == 0) {
		if (excl == EXCL)
			error = EEXIST;
		else if (vp->v_type == VDIR && (mode & VWRITE))
			error = EISDIR;
		else
			error = VOP_ACCESS(vp, mode, 0, cred, ct);

		if (error) {
			VN_RELE(vp);
		} else
			*vpp = vp;
	} else if (error == ENOENT) {
		error = EROFS;
	}

	return (error);
}
