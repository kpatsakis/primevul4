void safe_copy(char *dest, char *value, size_t buflen, const char *description) {
	int content_len = buflen - 1;

	if (strlen(value) >= content_len) {
		fprintf(stderr, "'%s' exceeds maximum %s length of %d\n",
			value, description, content_len);
		exit(EXIT_FAILURE);
	}
	strncpy(dest, value, content_len);
	dest[content_len] = 0;
}