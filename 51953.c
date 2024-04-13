MagickExport DrawInfo *AcquireDrawInfo(void)
{
  DrawInfo
    *draw_info;

  draw_info=(DrawInfo *) AcquireMagickMemory(sizeof(*draw_info));
  if (draw_info == (DrawInfo *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  GetDrawInfo((ImageInfo *) NULL,draw_info);
  return(draw_info);
}
