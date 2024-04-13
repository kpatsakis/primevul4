int fibw_count(void) {
	int rv = 0;
	IFBW *ptr = ifbw;

	while (ptr) {
		rv++;
		ptr = ptr->next;
	}
	
	return rv;
}
