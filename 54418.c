int HTLoadError(HTStream *sink GCC_UNUSED, int number,
		const char *message)
{
    HTAlert(message);		/* @@@@@@@@@@@@@@@@@@@ */
    return -number;
}
