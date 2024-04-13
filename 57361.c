static bool is_bool(const char *s) {
	return !strcasecmp ("true", s) || !strcasecmp ("false", s);
}
