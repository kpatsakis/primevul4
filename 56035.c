static void InsertComplexDoubleRow(Image *image,double *p,int y,double MinVal,
  double MaxVal,ExceptionInfo *exception)
{

  double f;
  int x;
  register Quantum *q;

  if (MinVal == 0)
    MinVal = -1;
  if (MaxVal == 0)
    MaxVal = 1;

  q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
  if (q == (Quantum *) NULL)
    return;
  for (x = 0; x < (ssize_t) image->columns; x++)
  {
    if (*p > 0)
    {
      f = (*p / MaxVal) * (QuantumRange-GetPixelRed(image,q));
      if (f + GetPixelRed(image,q) > QuantumRange)
        SetPixelRed(image,QuantumRange,q);
      else
        SetPixelRed(image,GetPixelRed(image,q)+(int) f,q);
      if ((int) f / 2.0 > GetPixelGreen(image,q))
        {
          SetPixelGreen(image,0,q);
          SetPixelBlue(image,0,q);
        }
      else
        {
          SetPixelBlue(image,GetPixelBlue(image,q)-(int) (f/2.0),q);
          SetPixelGreen(image,GetPixelBlue(image,q),q);
        }
    }
    if (*p < 0)
    {
      f = (*p / MaxVal) * (QuantumRange-GetPixelBlue(image,q));
      if (f+GetPixelBlue(image,q) > QuantumRange)
        SetPixelBlue(image,QuantumRange,q);
      else
        SetPixelBlue(image,GetPixelBlue(image,q)+(int) f,q);
      if ((int) f / 2.0 > GetPixelGreen(image,q))
        {
          SetPixelRed(image,0,q);
          SetPixelGreen(image,0,q);
        }
      else
        {
          SetPixelRed(image,GetPixelRed(image,q)-(int) (f/2.0),q);
          SetPixelGreen(image,GetPixelRed(image,q),q);
        }
    }
    p++;
    q+=GetPixelChannels(image);
  }
  if (!SyncAuthenticPixels(image,exception))
    return;
  return;
}
