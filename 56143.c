print_vendor_attr(netdissect_options *ndo,
                  register const u_char *data, u_int length, u_short attr_code _U_)
{
    u_int idx;
    u_int vendor_id;
    u_int vendor_type;
    u_int vendor_length;

    if (length < 4)
        goto trunc;
    ND_TCHECK2(*data, 4);
    vendor_id = EXTRACT_32BITS(data);
    data+=4;
    length-=4;

    ND_PRINT((ndo, "Vendor: %s (%u)",
           tok2str(smi_values,"Unknown",vendor_id),
           vendor_id));

    while (length >= 2) {
	ND_TCHECK2(*data, 2);

        vendor_type = *(data);
        vendor_length = *(data+1);

        if (vendor_length < 2)
        {
            ND_PRINT((ndo, "\n\t    Vendor Attribute: %u, Length: %u (bogus, must be >= 2)",
                   vendor_type,
                   vendor_length));
            return;
        }
        if (vendor_length > length)
        {
            ND_PRINT((ndo, "\n\t    Vendor Attribute: %u, Length: %u (bogus, goes past end of vendor-specific attribute)",
                   vendor_type,
                   vendor_length));
            return;
        }
        data+=2;
        vendor_length-=2;
        length-=2;
	ND_TCHECK2(*data, vendor_length);

        ND_PRINT((ndo, "\n\t    Vendor Attribute: %u, Length: %u, Value: ",
               vendor_type,
               vendor_length));
        for (idx = 0; idx < vendor_length ; idx++, data++)
            ND_PRINT((ndo, "%c", (*data < 32 || *data > 126) ? '.' : *data));
        length-=vendor_length;
    }
    return;

   trunc:
     ND_PRINT((ndo, "%s", tstr));
}
