void batadv_frag_purge_orig(struct batadv_orig_node *orig_node,
			    bool (*check_cb)(struct batadv_frag_table_entry *))
{
	struct batadv_frag_table_entry *chain;
	uint8_t i;

	for (i = 0; i < BATADV_FRAG_BUFFER_COUNT; i++) {
		chain = &orig_node->fragments[i];
		spin_lock_bh(&orig_node->fragments[i].lock);

		if (!check_cb || check_cb(chain)) {
			batadv_frag_clear_chain(&orig_node->fragments[i].head);
			orig_node->fragments[i].size = 0;
		}

		spin_unlock_bh(&orig_node->fragments[i].lock);
	}
}
