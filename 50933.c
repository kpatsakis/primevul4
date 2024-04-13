static void implement(const struct hid_device *hid, u8 *report,
		      unsigned offset, unsigned n, u32 value)
{
	u64 m;

	if (n > 32) {
		hid_warn(hid, "%s() called with n (%d) > 32! (%s)\n",
			 __func__, n, current->comm);
		n = 32;
	}

	m = (1ULL << n) - 1;
	if (value > m)
		hid_warn(hid, "%s() called with too large value %d! (%s)\n",
			 __func__, value, current->comm);
	WARN_ON(value > m);
	value &= m;

	__implement(report, offset, n, value);
}
