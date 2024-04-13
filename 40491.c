static void __exit crypto_user_exit(void)
{
	netlink_kernel_release(crypto_nlsk);
}
