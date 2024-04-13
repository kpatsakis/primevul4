static void batadv_frag_clear_chain(struct hlist_head *head)
{
	struct batadv_frag_list_entry *entry;
	struct hlist_node *node;

	hlist_for_each_entry_safe(entry, node, head, list) {
		hlist_del(&entry->list);
		kfree_skb(entry->skb);
		kfree(entry);
	}
}
