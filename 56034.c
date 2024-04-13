static void FixSignedValues(const Image *image,Quantum *q, int y)
{
  while(y-->0)
  {
     /* Please note that negative values will overflow
        Q=8; QuantumRange=255: <0;127> + 127+1 = <128; 255>
           <-1;-128> + 127+1 = <0; 127> */
    SetPixelRed(image,GetPixelRed(image,q)+QuantumRange/2+1,q);
    SetPixelGreen(image,GetPixelGreen(image,q)+QuantumRange/2+1,q);
    SetPixelBlue(image,GetPixelBlue(image,q)+QuantumRange/2+1,q);
    q++;
  }
}
