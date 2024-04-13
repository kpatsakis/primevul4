static int StopInfoCompare(const void *x,const void *y)
{
  StopInfo
    *stop_1,
    *stop_2;

  stop_1=(StopInfo *) x;
  stop_2=(StopInfo *) y;
  if (stop_1->offset > stop_2->offset)
    return(1);
  if (fabs(stop_1->offset-stop_2->offset) <= MagickEpsilon)
    return(0);
  return(-1);
}
