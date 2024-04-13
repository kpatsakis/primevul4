void ef_globals_alloc(void)
{

   SAFE_CALLOC(ef_gbls, 1, sizeof(struct ef_globals));

   return;
}
