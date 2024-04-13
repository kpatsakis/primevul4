static void __ipxitf_put(struct ipx_interface *intrfc)
{
	if (atomic_dec_and_test(&intrfc->refcnt))
		__ipxitf_down(intrfc);
}
