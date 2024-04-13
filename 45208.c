int compat_mc_setsockopt(struct sock *sock, int level, int optname,
	char __user *optval, unsigned int optlen,
	int (*setsockopt)(struct sock *, int, int, char __user *, unsigned int))
{
	char __user	*koptval = optval;
	int		koptlen = optlen;

	switch (optname) {
	case MCAST_JOIN_GROUP:
	case MCAST_LEAVE_GROUP:
	{
		struct compat_group_req __user *gr32 = (void *)optval;
		struct group_req __user *kgr =
			compat_alloc_user_space(sizeof(struct group_req));
		u32 interface;

		if (!access_ok(VERIFY_READ, gr32, sizeof(*gr32)) ||
		    !access_ok(VERIFY_WRITE, kgr, sizeof(struct group_req)) ||
		    __get_user(interface, &gr32->gr_interface) ||
		    __put_user(interface, &kgr->gr_interface) ||
		    copy_in_user(&kgr->gr_group, &gr32->gr_group,
				sizeof(kgr->gr_group)))
			return -EFAULT;
		koptval = (char __user *)kgr;
		koptlen = sizeof(struct group_req);
		break;
	}
	case MCAST_JOIN_SOURCE_GROUP:
	case MCAST_LEAVE_SOURCE_GROUP:
	case MCAST_BLOCK_SOURCE:
	case MCAST_UNBLOCK_SOURCE:
	{
		struct compat_group_source_req __user *gsr32 = (void *)optval;
		struct group_source_req __user *kgsr = compat_alloc_user_space(
			sizeof(struct group_source_req));
		u32 interface;

		if (!access_ok(VERIFY_READ, gsr32, sizeof(*gsr32)) ||
		    !access_ok(VERIFY_WRITE, kgsr,
			sizeof(struct group_source_req)) ||
		    __get_user(interface, &gsr32->gsr_interface) ||
		    __put_user(interface, &kgsr->gsr_interface) ||
		    copy_in_user(&kgsr->gsr_group, &gsr32->gsr_group,
				sizeof(kgsr->gsr_group)) ||
		    copy_in_user(&kgsr->gsr_source, &gsr32->gsr_source,
				sizeof(kgsr->gsr_source)))
			return -EFAULT;
		koptval = (char __user *)kgsr;
		koptlen = sizeof(struct group_source_req);
		break;
	}
	case MCAST_MSFILTER:
	{
		struct compat_group_filter __user *gf32 = (void *)optval;
		struct group_filter __user *kgf;
		u32 interface, fmode, numsrc;

		if (!access_ok(VERIFY_READ, gf32, __COMPAT_GF0_SIZE) ||
		    __get_user(interface, &gf32->gf_interface) ||
		    __get_user(fmode, &gf32->gf_fmode) ||
		    __get_user(numsrc, &gf32->gf_numsrc))
			return -EFAULT;
		koptlen = optlen + sizeof(struct group_filter) -
				sizeof(struct compat_group_filter);
		if (koptlen < GROUP_FILTER_SIZE(numsrc))
			return -EINVAL;
		kgf = compat_alloc_user_space(koptlen);
		if (!access_ok(VERIFY_WRITE, kgf, koptlen) ||
		    __put_user(interface, &kgf->gf_interface) ||
		    __put_user(fmode, &kgf->gf_fmode) ||
		    __put_user(numsrc, &kgf->gf_numsrc) ||
		    copy_in_user(&kgf->gf_group, &gf32->gf_group,
				sizeof(kgf->gf_group)) ||
		    (numsrc && copy_in_user(kgf->gf_slist, gf32->gf_slist,
				numsrc * sizeof(kgf->gf_slist[0]))))
			return -EFAULT;
		koptval = (char __user *)kgf;
		break;
	}

	default:
		break;
	}
	return setsockopt(sock, level, optname, koptval, koptlen);
}
