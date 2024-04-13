void CLASS parse_thumb_note (int base, unsigned toff, unsigned tlen)
{
  unsigned entries, tag, type, len, save;

  entries = get2();
  while (entries--) {
    tiff_get (base, &tag, &type, &len, &save);
    if (tag == toff) thumb_offset = get4()+base;
    if (tag == tlen) thumb_length = get4();
    fseek (ifp, save, SEEK_SET);
  }
}
