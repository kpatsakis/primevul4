int register_md_personality(struct md_personality *p)
{
	printk(KERN_INFO "md: %s personality registered for level %d\n",
						p->name, p->level);
	spin_lock(&pers_lock);
	list_add_tail(&p->list, &pers_list);
	spin_unlock(&pers_lock);
	return 0;
}
