fileTimeToUtc(uint64_t fileTime, time_t *timep, long *ns)
{

	if (fileTime >= EPOC_TIME) {
		fileTime -= EPOC_TIME;
		/* milli seconds base */
		*timep = (time_t)(fileTime / 10000000);
		/* nano seconds base */
		*ns = (long)(fileTime % 10000000) * 100;
	} else {
		*timep = 0;
		*ns = 0;
	}
}
