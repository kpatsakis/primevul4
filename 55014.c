static bool fanout_find_new_id(struct sock *sk, u16 *new_id)
{
	u16 id = fanout_next_id;

	do {
		if (__fanout_id_is_free(sk, id)) {
			*new_id = id;
			fanout_next_id = id + 1;
			return true;
		}

		id++;
	} while (id != fanout_next_id);

	return false;
}
