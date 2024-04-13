static int call_usermodehelper_keys(const char *path, char **argv, char **envp,
					struct key *session_keyring, int wait)
{
	struct subprocess_info *info;

	info = call_usermodehelper_setup(path, argv, envp, GFP_KERNEL,
					  umh_keys_init, umh_keys_cleanup,
					  session_keyring);
	if (!info)
		return -ENOMEM;

	key_get(session_keyring);
	return call_usermodehelper_exec(info, wait);
}
