static unsigned LoadWPG2Flags(Image *image,char Precision,float *Angle,tCTM *CTM)
{
const unsigned char TPR=1,TRN=2,SKW=4,SCL=8,ROT=0x10,OID=0x20,LCK=0x80;
ssize_t x;
unsigned DenX;
unsigned Flags;

 (void) memset(*CTM,0,sizeof(*CTM));     /*CTM.erase();CTM.resize(3,3);*/
 (*CTM)[0][0]=1;
 (*CTM)[1][1]=1;
 (*CTM)[2][2]=1;

 Flags=ReadBlobLSBShort(image);
 if(Flags & LCK) (void) ReadBlobLSBLong(image);  /*Edit lock*/
 if(Flags & OID)
  {
  if(Precision==0)
    {(void) ReadBlobLSBShort(image);}  /*ObjectID*/
  else
    {(void) ReadBlobLSBLong(image);}  /*ObjectID (Double precision)*/
  }
 if(Flags & ROT)
  {
  x=ReadBlobLSBLong(image);  /*Rot Angle*/
  if(Angle) *Angle=x/65536.0;
  }
 if(Flags & (ROT|SCL))
  {
  x=ReadBlobLSBLong(image);  /*Sx*cos()*/
  (*CTM)[0][0] = (float)x/0x10000;
  x=ReadBlobLSBLong(image);  /*Sy*cos()*/
  (*CTM)[1][1] = (float)x/0x10000;
  }
 if(Flags & (ROT|SKW))
  {
  x=ReadBlobLSBLong(image);       /*Kx*sin()*/
  (*CTM)[1][0] = (float)x/0x10000;
  x=ReadBlobLSBLong(image);       /*Ky*sin()*/
  (*CTM)[0][1] = (float)x/0x10000;
  }
 if(Flags & TRN)
  {
  x=ReadBlobLSBLong(image); DenX=ReadBlobLSBShort(image);  /*Tx*/
        if(x>=0) (*CTM)[0][2] = (float)x+(float)DenX/0x10000;
            else (*CTM)[0][2] = (float)x-(float)DenX/0x10000;
  x=ReadBlobLSBLong(image); DenX=ReadBlobLSBShort(image);  /*Ty*/
  (*CTM)[1][2]=(float)x + ((x>=0)?1:-1)*(float)DenX/0x10000;
        if(x>=0) (*CTM)[1][2] = (float)x+(float)DenX/0x10000;
            else (*CTM)[1][2] = (float)x-(float)DenX/0x10000;
  }
 if(Flags & TPR)
  {
  x=ReadBlobLSBShort(image); DenX=ReadBlobLSBShort(image);  /*Px*/
  (*CTM)[2][0] = x + (float)DenX/0x10000;;
  x=ReadBlobLSBShort(image);  DenX=ReadBlobLSBShort(image); /*Py*/
  (*CTM)[2][1] = x + (float)DenX/0x10000;
  }
 return(Flags);
}
