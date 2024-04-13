mtree_atol8(char **p)
{
	int64_t	l, limit, last_digit_limit;
	int digit, base;

	base = 8;
	limit = INT64_MAX / base;
	last_digit_limit = INT64_MAX % base;

	l = 0;
	digit = **p - '0';
	while (digit >= 0 && digit < base) {
		if (l>limit || (l == limit && digit > last_digit_limit)) {
			l = INT64_MAX; /* Truncate on overflow. */
			break;
		}
		l = (l * base) + digit;
		digit = *++(*p) - '0';
	}
	return (l);
}
