void __dev_remove_pack(struct packet_type *pt)
{
	struct list_head *head = ptype_head(pt);
	struct packet_type *pt1;

	spin_lock(&ptype_lock);

	list_for_each_entry(pt1, head, list) {
		if (pt == pt1) {
			list_del_rcu(&pt->list);
			goto out;
		}
	}

	printk(KERN_WARNING "dev_remove_pack: %p not found.\n", pt);
out:
	spin_unlock(&ptype_lock);
}
