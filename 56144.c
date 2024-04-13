radius_attrs_print(netdissect_options *ndo,
                   register const u_char *attr, u_int length)
{
   register const struct radius_attr *rad_attr = (const struct radius_attr *)attr;
   const char *attr_string;

   while (length > 0)
   {
     if (length < 2)
        goto trunc;
     ND_TCHECK(*rad_attr);

     if (rad_attr->type > 0 && rad_attr->type < TAM_SIZE(attr_type))
	attr_string = attr_type[rad_attr->type].name;
     else
	attr_string = "Unknown";
     if (rad_attr->len < 2)
     {
	ND_PRINT((ndo, "\n\t  %s Attribute (%u), length: %u (bogus, must be >= 2)",
               attr_string,
               rad_attr->type,
               rad_attr->len));
	return;
     }
     if (rad_attr->len > length)
     {
	ND_PRINT((ndo, "\n\t  %s Attribute (%u), length: %u (bogus, goes past end of packet)",
               attr_string,
               rad_attr->type,
               rad_attr->len));
        return;
     }
     ND_PRINT((ndo, "\n\t  %s Attribute (%u), length: %u, Value: ",
            attr_string,
            rad_attr->type,
            rad_attr->len));

     if (rad_attr->type < TAM_SIZE(attr_type))
     {
         if (rad_attr->len > 2)
         {
             if ( attr_type[rad_attr->type].print_func )
                 (*attr_type[rad_attr->type].print_func)(
                     ndo, ((const u_char *)(rad_attr+1)),
                     rad_attr->len - 2, rad_attr->type);
         }
     }
     /* do we also want to see a hex dump ? */
     if (ndo->ndo_vflag> 1)
         print_unknown_data(ndo, (const u_char *)rad_attr+2, "\n\t    ", (rad_attr->len)-2);

     length-=(rad_attr->len);
     rad_attr = (const struct radius_attr *)( ((const char *)(rad_attr))+rad_attr->len);
   }
   return;

trunc:
   ND_PRINT((ndo, "%s", tstr));
}
