static int __init init_root_keyring(void)
{
	return install_user_keyrings();
}
