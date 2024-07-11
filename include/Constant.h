#ifndef Constant_h
#define Constant_h

namespace RICHsystem{
	//Important: Constants changed
  //const int Nfec = 4;
  //const int Nchip = 4;
  //const int Nch =64;

  const int Nfec = 2;
  const int Nchip = 4;
  const int Nch = 68;


  const int Nsp =512; //sampling points
  const int Tchip = Nfec*Nchip;
  const int Tch = Tchip*Nch;
  const int Nconnector =8;
  const int Ncchn=128;
}
#endif
