static size_t add_data_segment(u_char **data, size_t base, u_char **string, size_t slen)
{
   /* make room for the new string */
   SAFE_REALLOC(*data, base + slen + 1);

   /* copy the string, NULL separated */
   memcpy(*data + base, *string, slen + 1);

   /* 
    * change the pointer to the new string location 
    * it is an offset from the base of the data segment
    */
   *string = (u_char *)base;
   
   /* retur the len of the added string */
   return slen + 1;
}
