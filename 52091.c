void unregister_key_type(struct key_type *ktype)
{
	down_write(&key_types_sem);
	list_del_init(&ktype->link);
	downgrade_write(&key_types_sem);
	key_gc_keytype(ktype);
	pr_notice("Key type %s unregistered\n", ktype->name);
	up_read(&key_types_sem);
}
