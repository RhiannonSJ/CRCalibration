#include "ExternalFunctionHelpers.h"

namespace calib{
  
  // --------------------------------------------------------------------------------------------------------------------------------------------------

  double langaufun(double *x, double *par) {

    //From here: https://root.cern.ch/root/html534/tutorials/fit/langaus.C.html
    //Fit parameters:
    //par[0]=Width (scale) parameter of Landau density
    //par[1]=Most Probable (MP, location) parameter of Landau density
    //par[2]=Total area (integral -inf to inf, normalization constant)
    //par[3]=Width (sigma) of convoluted Gaussian function
    //
    //In the Landau distribution (represented by the CERNLIB approximation), 
    //the maximum is located at x=-0.22278298 with the location parameter=0.
    //This shift is corrected within this function, so that the actual
    //maximum is identical to the MP parameter.

    // Numeric constants
    double invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
    double mpshift  = -0.22278298;       // Landau maximum location

    // Control constants
    double np = 100.0;      // number of convolution steps
    double sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

    // Variables
    double xx;
    double mpc;
    double fland;
    double sum = 0.0;
    double xlow,xupp;
    double step;
    double i;

    // MP shift correction
    mpc = par[1] - mpshift * par[0]; 

    // Range of convolution integral
    xlow = x[0] - sc * par[3];
    xupp = x[0] + sc * par[3];

    step = (xupp-xlow) / np;

    // Convolution integral of Landau and Gaussian by sum
    for(i=1.0; i<=np/2; i++) {
      xx = xlow + (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);

      xx = xupp - (i-.5) * step;
      fland = TMath::Landau(xx,mpc,par[0]) / par[0];
      sum += fland * TMath::Gaus(x[0],xx,par[3]);
    }

    return (par[2] * step * sum * invsq2pi / par[3]);
  }

  // --------------------------------------------------------------------------------------------------------------------------------------------------
  
  int langaupro(double *params, double &maxx, double &FWHM) {

    //From here: https://root.cern.ch/root/html534/tutorials/fit/langaus.C.html
    // Seaches for the location (x value) at the maximum of the 
    // Landau-Gaussian convolute and its full width at half-maximum.
    //
    // The search is probably not very efficient, but it's a first try.
    double p,x,fy,fxr,fxl;
    double step;
    double l,lold;
    int i = 0;
    int MAXCALLS = 10000;

    // Search for maximum
    p = params[1] - 0.1 * params[0];
    step = 0.05 * params[0];
    lold = -2.0;
    l    = -1.0;

    while ( (l != lold) && (i < MAXCALLS) ) {
      i++;

      lold = l;
      x = p + step;
      l = langaufun(&x,params);

      if (l < lold)
        step = -step/10;
      
      p += step;
    }

    if (i == MAXCALLS)
      return (-1);

    maxx = x;
    fy = l/2;
    std::cout << " maxx: " << maxx << ", l: " << l << ", l/2: " << fy << std::endl;

    // Search for right x location of fy
    p = maxx + params[0];
    step = params[0];
    lold = -2.0;
    l    = -1e300;
    i    = 0;


    while ( (l != lold) && (i < MAXCALLS) ) {
      i++;

      lold = l;
      x = p + step;
      l = TMath::Abs(langaufun(&x,params) - fy);

      if (l > lold)
        step = -step/10;

      p += step;
    }

    if (i == MAXCALLS)
      return (-2);

    fxr = x;

    // Search for left x location of fy
    p = maxx - 0.5 * params[0];
    step = -params[0];
    lold = -2.0;
    l    = -1e300;
    i    = 0;

    while ( (l != lold) && (i < MAXCALLS) ) {
      i++;

      lold = l;
      x = p + step;
      l = TMath::Abs(langaufun(&x,params) - fy);

      if (l > lold)
        step = -step/10;

      p += step;
    }

    if (i == MAXCALLS)
      return (-3);

    fxl = x;

    FWHM = fxr - fxl;

    std::cout << " fxl: " << fxl << ", fxr: " << fxr << ", fwhm: " << FWHM << std::endl;
    std::cin.get();
    return (0);
  } // langaupro

  // --------------------------------------------------------------------------------------------------------------------------------------------------
  
  double ModBoxCorrection(const double &dQdx, const bool &tuned, const double &eField){
    // Modified Box model correction has better behavior than the Birks
    // correction at high values of dQ/dx.
    constexpr double Rho   = calib::kArDensity; // LAr density in g/cm^3
    constexpr double Wion  = calib::kWion;      // 23.6 eV = 1e, Wion in MeV/e
    double const Beta      = calib::kModBoxB / (Rho * eField);
    constexpr double Alpha = calib::kModBoxA;
    double scale           = calib::kElectronsToADC;
    if(tuned)
      scale = calib::kElectronsToADCTuned;
    double const dEdx      = (exp(Beta * Wion * (dQdx/scale)) - Alpha) / Beta; 

    return dEdx;
 
  }

  // --------------------------------------------------------------------------------------------------------------------------------------------------
  
} // calib
