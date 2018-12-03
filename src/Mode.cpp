#include "Mode.h"

Mode::Mode()
{
  I = MatrixXd::Identity(2,2);
  A1 = MatrixXd::Zero(2,2);
  A2 = MatrixXd::Zero(2,2);
  B0 = MatrixXd::Zero(2,2);
  B1 = MatrixXd::Zero(2,2);
  B2 = MatrixXd::Zero(2,2);
  B = MatrixXd::Zero(2,2);
  omega = MatrixXd::Zero(2,1);
  lambda = MatrixXd::Zero(2,1);
  gamma = MatrixXd::Zero(2,1);

  rho = MatrixXd::Zero(2,1);
  Q = MatrixXd::Zero(2,2);
}

Mode::Mode(const Mode&)
{
}

Mode::~Mode()
{
}

void Mode::GetDensity(MatrixXd u, MatrixXd density)
{  
  rho = (I + omega(0,0) * A1 + omega(1,0) * A2) * density + (B0 + omega(0,0) * B1 + omega(1,0) * B2) * lambda + B * u;
}

void Mode::GetAutoCorrelation(MatrixXd u,  MatrixXd density, MatrixXd prev_Q)
{
  G1 = MatrixXd::Zero(2,2);
  G2 = MatrixXd::Zero(2,2);
  G3 = MatrixXd::Zero(2,2);
  G = MatrixXd::Zero(2,2);
  V = MatrixXd::Zero(2,2);
 
  MatrixXd W = omega(0,0) * A1 + omega(1,0) * A2;
  MatrixXd D = B0 + omega(0,0) * B1 + omega(1,0) * B2;

  G1 = (I + W) * density * u.transpose() * B.transpose();
  G2 = D * lambda * u.transpose() * B.transpose();
  G3 = (I + W) * density * lambda.transpose() * D.transpose()
  + gamma(0,0) * A1 * density * lambda.transpose() * B1.transpose()
  + gamma(1,0) * A2 * density * lambda.transpose() * B2.transpose();
 
  G = G1 + G2 + G3;

  V = G + G.transpose()
  + B * u * u.transpose() * B.transpose()
  + D * lambda * lambda.transpose() * D.transpose()
  + gamma(0,0) * B1 * lambda * lambda.transpose() * B1.transpose()
  + gamma(1,0) * B2 * lambda * lambda.transpose() * B2.transpose();

  Q = (I + W) * prev_Q * (I + W).transpose()
    + gamma(0,0) * A1 * prev_Q * A1.transpose()
    + gamma(1,0) * A2 * prev_Q * A2.transpose()
    + V;
}
