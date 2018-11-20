#ifndef __SCTMsim__Mode__
#define __SCTMsim__Mode__

#include "setting.h"

class Mode
{
 public:
  Mode();
  Mode(const Mode&);
  ~Mode();

  void GetDensity(MatrixXd, MatrixXd);
  void GetAutoCorrelation(MatrixXd, MatrixXd, MatrixXd);

  MatrixXd A1;
  MatrixXd A2;
  MatrixXd B0;
  MatrixXd B1;
  MatrixXd B2;
  MatrixXd B;
  MatrixXd I;
  MatrixXd omega;
  MatrixXd lambda;
  MatrixXd gamma;

  MatrixXd rho;
  MatrixXd G1;
  MatrixXd G2; 
  MatrixXd G3; 
  MatrixXd G; 
  MatrixXd V;
  MatrixXd Q;
};

#endif /* defined(__SCTMsim__Mode__) */
