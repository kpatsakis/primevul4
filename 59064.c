void ef_globals_free(void)
{
   SAFE_FREE(ef_gbls->source_file);
   SAFE_FREE(ef_gbls->output_file);
   SAFE_FREE(ef_gbls);

   return;

}
