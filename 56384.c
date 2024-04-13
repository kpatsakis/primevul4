static inline double MagickRound(double x)
{
  /*
    Round the fraction to nearest integer.
  */
  if ((x-floor(x)) < (ceil(x)-x))
    return(floor(x));
  return(ceil(x));
}
