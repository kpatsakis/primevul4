int compat_mc_getsockopt(struct sock *sock, int level, int optname,
	char __user *optval, int __user *optlen,
	int (*getsockopt)(struct sock *, int, int, char __user *, int __user *))
{
	struct compat_group_filter __user *gf32 = (void *)optval;
	struct group_filter __user *kgf;
	int __user	*koptlen;
	u32 interface, fmode, numsrc;
	int klen, ulen, err;

	if (optname != MCAST_MSFILTER)
		return getsockopt(sock, level, optname, optval, optlen);

	koptlen = compat_alloc_user_space(sizeof(*koptlen));
	if (!access_ok(VERIFY_READ, optlen, sizeof(*optlen)) ||
	    __get_user(ulen, optlen))
		return -EFAULT;

	/* adjust len for pad */
	klen = ulen + sizeof(*kgf) - sizeof(*gf32);

	if (klen < GROUP_FILTER_SIZE(0))
		return -EINVAL;

	if (!access_ok(VERIFY_WRITE, koptlen, sizeof(*koptlen)) ||
	    __put_user(klen, koptlen))
		return -EFAULT;

	/* have to allow space for previous compat_alloc_user_space, too */
	kgf = compat_alloc_user_space(klen+sizeof(*optlen));

	if (!access_ok(VERIFY_READ, gf32, __COMPAT_GF0_SIZE) ||
	    __get_user(interface, &gf32->gf_interface) ||
	    __get_user(fmode, &gf32->gf_fmode) ||
	    __get_user(numsrc, &gf32->gf_numsrc) ||
	    __put_user(interface, &kgf->gf_interface) ||
	    __put_user(fmode, &kgf->gf_fmode) ||
	    __put_user(numsrc, &kgf->gf_numsrc) ||
	    copy_in_user(&kgf->gf_group, &gf32->gf_group, sizeof(kgf->gf_group)))
		return -EFAULT;

	err = getsockopt(sock, level, optname, (char __user *)kgf, koptlen);
	if (err)
		return err;

	if (!access_ok(VERIFY_READ, koptlen, sizeof(*koptlen)) ||
	    __get_user(klen, koptlen))
		return -EFAULT;

	ulen = klen - (sizeof(*kgf)-sizeof(*gf32));

	if (!access_ok(VERIFY_WRITE, optlen, sizeof(*optlen)) ||
	    __put_user(ulen, optlen))
		return -EFAULT;

	if (!access_ok(VERIFY_READ, kgf, klen) ||
	    !access_ok(VERIFY_WRITE, gf32, ulen) ||
	    __get_user(interface, &kgf->gf_interface) ||
	    __get_user(fmode, &kgf->gf_fmode) ||
	    __get_user(numsrc, &kgf->gf_numsrc) ||
	    __put_user(interface, &gf32->gf_interface) ||
	    __put_user(fmode, &gf32->gf_fmode) ||
	    __put_user(numsrc, &gf32->gf_numsrc))
		return -EFAULT;
	if (numsrc) {
		int copylen;

		klen -= GROUP_FILTER_SIZE(0);
		copylen = numsrc * sizeof(gf32->gf_slist[0]);
		if (copylen > klen)
			copylen = klen;
		if (copy_in_user(gf32->gf_slist, kgf->gf_slist, copylen))
			return -EFAULT;
	}
	return err;
}
