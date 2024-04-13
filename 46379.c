monitor_reset_key_state(void)
{
	/* reset state */
	free(key_blob);
	free(hostbased_cuser);
	free(hostbased_chost);
	key_blob = NULL;
	key_bloblen = 0;
	key_blobtype = MM_NOKEY;
	hostbased_cuser = NULL;
	hostbased_chost = NULL;
}
