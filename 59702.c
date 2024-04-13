void ifbw_remove(IFBW *r) {
	if (ifbw == NULL)
		return;
	
	if (ifbw == r) {
		ifbw = ifbw->next;
		return;
	}
	
	IFBW *ptr = ifbw->next;
	IFBW *prev = ifbw;
	while (ptr) {
		if (ptr == r) {
			prev->next = ptr->next;
			return;
		}
		
		prev = ptr;
		ptr = ptr->next;
	}	
	
	return;
}
