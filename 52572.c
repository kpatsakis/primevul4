static spl_ptr_llist *spl_ptr_llist_init(spl_ptr_llist_ctor_func ctor, spl_ptr_llist_dtor_func dtor) /* {{{ */
{
	spl_ptr_llist *llist = emalloc(sizeof(spl_ptr_llist));

	llist->head  = NULL;
	llist->tail  = NULL;
	llist->count = 0;
	llist->dtor  = dtor;
	llist->ctor  = ctor;

	return llist;
}
/* }}} */
