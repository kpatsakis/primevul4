static int get_serial_usage(struct acm *acm,
			    struct serial_icounter_struct __user *count)
{
	struct serial_icounter_struct icount;
	int rv = 0;

	memset(&icount, 0, sizeof(icount));
	icount.dsr = acm->iocount.dsr;
	icount.rng = acm->iocount.rng;
	icount.dcd = acm->iocount.dcd;
	icount.frame = acm->iocount.frame;
	icount.overrun = acm->iocount.overrun;
	icount.parity = acm->iocount.parity;
	icount.brk = acm->iocount.brk;

	if (copy_to_user(count, &icount, sizeof(icount)) > 0)
		rv = -EFAULT;

	return rv;
}
