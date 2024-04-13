static void RemoveICCProfileFromResourceBlock(StringInfo *bim_profile)
{
  register const unsigned char
    *p;

  size_t
    length;

  unsigned char
    *datum;

  unsigned int
    count,
    long_sans;

  unsigned short
    id,
    short_sans;

  length=GetStringInfoLength(bim_profile);
  if (length < 16)
    return;
  datum=GetStringInfoDatum(bim_profile);
  for (p=datum; (p >= datum) && (p < (datum+length-16)); )
  {
    register unsigned char
      *q;

    q=(unsigned char *) p;
    if (LocaleNCompare((const char *) p,"8BIM",4) != 0)
      break;
    p=PushLongPixel(MSBEndian,p,&long_sans);
    p=PushShortPixel(MSBEndian,p,&id);
    p=PushShortPixel(MSBEndian,p,&short_sans);
    p=PushLongPixel(MSBEndian,p,&count);
    if (id == 0x0000040f)
      {
        ssize_t
          quantum;

        quantum=PSDQuantum(count)+12;
        if ((quantum >= 12) && (quantum < (ssize_t) length))
          {
            if ((q+quantum < (datum+length-16)))
              (void) CopyMagickMemory(q,q+quantum,length-quantum-(q-datum));
            SetStringInfoLength(bim_profile,length-quantum);
          }
        break;
      }
    p+=count;
    if ((count & 0x01) != 0)
      p++;
  }
}
