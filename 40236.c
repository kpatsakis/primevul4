static int iscsi_check_for_auth_key(char *key)
{
	/*
	 * RFC 1994
	 */
	if (!strcmp(key, "CHAP_A") || !strcmp(key, "CHAP_I") ||
	    !strcmp(key, "CHAP_C") || !strcmp(key, "CHAP_N") ||
	    !strcmp(key, "CHAP_R"))
		return 1;

	/*
	 * RFC 2945
	 */
	if (!strcmp(key, "SRP_U") || !strcmp(key, "SRP_N") ||
	    !strcmp(key, "SRP_g") || !strcmp(key, "SRP_s") ||
	    !strcmp(key, "SRP_A") || !strcmp(key, "SRP_B") ||
	    !strcmp(key, "SRP_M") || !strcmp(key, "SRP_HM"))
		return 1;

	return 0;
}
