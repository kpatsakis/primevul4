static int audit_in_mask(const struct audit_krule *rule, unsigned long val)
{
	int word, bit;

	if (val > 0xffffffff)
		return false;

	word = AUDIT_WORD(val);
	if (word >= AUDIT_BITMASK_SIZE)
		return false;

	bit = AUDIT_BIT(val);

	return rule->mask[word] & bit;
}
