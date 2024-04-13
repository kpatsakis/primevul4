MagickExport MagickBooleanType DeleteImageProfile(Image *image,const char *name)
{
  assert(image != (Image *) NULL);
  assert(image->signature == MagickSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (image->profiles == (SplayTreeInfo *) NULL)
    return(MagickFalse);
  if (LocaleCompare(name,"icc") == 0)
    {
      /*
        Continue to support deprecated color profile for now.
      */
      image->color_profile.length=0;
      image->color_profile.info=(unsigned char *) NULL;
    }
  if (LocaleCompare(name,"iptc") == 0)
    {
      /*
        Continue to support deprecated IPTC profile for now.
      */
      image->iptc_profile.length=0;
      image->iptc_profile.info=(unsigned char *) NULL;
    }
  WriteTo8BimProfile(image,name,(StringInfo *) NULL);
  return(DeleteNodeFromSplayTree((SplayTreeInfo *) image->profiles,name));
}
