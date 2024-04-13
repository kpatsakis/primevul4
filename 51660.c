static inline u32 vop_vringh_iov_consumed(struct vringh_kiov *iov)
{
	int i;
	u32 total = iov->consumed;

	for (i = 0; i < iov->i; i++)
		total += iov->iov[i].iov_len;
	return total;
}
