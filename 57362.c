R_API void r_config_bump(RConfig *cfg, const char *key) {
	char *orig = strdup (r_config_get (cfg, key));
	r_config_set (cfg, key, orig);
	free (orig);
}
