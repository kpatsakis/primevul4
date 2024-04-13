static void unix_sock_inherit_flags(const struct socket *old,
				    struct socket *new)
{
	if (test_bit(SOCK_PASSCRED, &old->flags))
		set_bit(SOCK_PASSCRED, &new->flags);
	if (test_bit(SOCK_PASSSEC, &old->flags))
		set_bit(SOCK_PASSSEC, &new->flags);
}
