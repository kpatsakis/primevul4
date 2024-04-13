static inline unsigned char ClampYCC(double value)
{
  value=255.0-value;
  if (value < 0.0)
    return((unsigned char)0);
  if (value > 255.0)
    return((unsigned char)255);
  return((unsigned char)(value));
}
