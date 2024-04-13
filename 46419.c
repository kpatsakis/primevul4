mm_user_key_allowed(struct passwd *pw, Key *key, int pubkey_auth_attempt)
{
	return (mm_key_allowed(MM_USERKEY, NULL, NULL, key,
	    pubkey_auth_attempt));
}
