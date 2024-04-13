mtree_atol10(char **p)
{
	int64_t l, limit, last_digit_limit;
	int base, digit, sign;

	base = 10;

	if (**p == '-') {
		sign = -1;
		limit = ((uint64_t)(INT64_MAX) + 1) / base;
		last_digit_limit = ((uint64_t)(INT64_MAX) + 1) % base;
		++(*p);
	} else {
		sign = 1;
		limit = INT64_MAX / base;
		last_digit_limit = INT64_MAX % base;
	}

	l = 0;
	digit = **p - '0';
	while (digit >= 0 && digit < base) {
		if (l > limit || (l == limit && digit > last_digit_limit))
			return (sign < 0) ? INT64_MIN : INT64_MAX;
		l = (l * base) + digit;
		digit = *++(*p) - '0';
	}
	return (sign < 0) ? -l : l;
}
