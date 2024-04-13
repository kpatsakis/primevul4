static const char *func_id_name(int id)
{
	BUILD_BUG_ON(ARRAY_SIZE(func_id_str) != __BPF_FUNC_MAX_ID);

	if (id >= 0 && id < __BPF_FUNC_MAX_ID && func_id_str[id])
		return func_id_str[id];
	else
		return "unknown";
}
