handle_is_ok(int i, int type)
{
	return i >= 0 && (u_int)i < num_handles && handles[i].use == type;
}
