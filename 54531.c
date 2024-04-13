_kdc_fix_time(time_t **t)
{
    if(*t == NULL){
	ALLOC(*t);
	**t = MAX_TIME;
    }
    if(**t == 0) **t = MAX_TIME; /* fix for old clients */
}
