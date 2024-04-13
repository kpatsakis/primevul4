static bool ovl_is_private_xattr(const char *name)
{
	return strncmp(name, OVL_XATTR_PRE_NAME, OVL_XATTR_PRE_LEN) == 0;
}
