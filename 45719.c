static struct sock_iocb *alloc_sock_iocb(struct kiocb *iocb,
					 struct sock_iocb *siocb)
{
	if (!is_sync_kiocb(iocb))
		BUG();

	siocb->kiocb = iocb;
	iocb->private = siocb;
	return siocb;
}
