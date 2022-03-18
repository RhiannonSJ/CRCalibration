#include "PlottingHelpers.h"

namespace calib{
  
  //------------------------------------------------------------------------------------------ 
  
  void SetCanvasStyle(TCanvas *c, const double &l, const double &r, const double &t, const double &b, const bool logX, const bool logY, const bool logZ){
    c->SetLeftMargin(l);
    c->SetRightMargin(r);
    c->SetTopMargin(t);
    c->SetBottomMargin(b);

    if(logX)
      c->SetLogx();
    if(logY)
      c->SetLogy();
    if(logZ)
      c->SetLogz();
  } // Canvas Style
  
  // --------------------------------------------------------------------------------------------------------------------------------------------------
  
  void SetUserPalette(){
    const int Number = 4;
    double Red[Number]    = {223/255., 137/255., 61/255., 26/255.};
    double Green[Number]  = {214/255., 119/255., 48/255., 21/255.};
    double Blue[Number]   = {234/255., 187/255., 95/255., 41/255.};
    double Length[Number] = {0, .45, .9, 1};
    /*
    double Red[Number]    = {26/255., 61/255., 137/255., 223/255.};
    double Green[Number]  = {21/255., 48/255., 119/255., 214/255.};
    double Blue[Number]   = {41/255., 95/255., 187/255., 234/255.};
    double Length[Number] = {0, .1, .55, 1};
    */
    int nb = 99;
    TColor::CreateGradientColorTable(Number,Length,Red,Green,Blue,nb);
  } // SetUserPalette
  
  // --------------------------------------------------------------------------------------------------------------------------------------------------
  
  void SetHistogramStyle1D(TH1D *h, const char *xLabel, const char *yLabel){
    h->GetXaxis()->SetTitle(xLabel);
    h->GetYaxis()->SetTitle(yLabel);
    h->GetXaxis()->SetTitleFont(132);
    h->GetXaxis()->SetLabelFont(132);
    h->GetYaxis()->SetTitleFont(132);
    h->GetYaxis()->SetLabelFont(132);
    h->GetXaxis()->SetTitleSize(0.055);
    h->GetXaxis()->SetLabelSize(0.045);
    h->GetYaxis()->SetTitleSize(0.055);
    h->GetYaxis()->SetLabelSize(0.045);
    h->GetXaxis()->SetMaxDigits(3);
    h->GetYaxis()->SetMaxDigits(3);
    h->GetYaxis()->SetTitleOffset(1.1);
    h->SetStats(0);

  } // 1D Histogram Style

  // --------------------------------------------------------------------------------------------------------------------------------------------------
  
  void SetHistogramStyle2D(TH2D *h, const char *xLabel, const char *yLabel, const bool &palDefault){
    h->GetXaxis()->SetTitle(xLabel);
    h->GetYaxis()->SetTitle(yLabel);
    h->GetXaxis()->SetTitleFont(132);
    h->GetXaxis()->SetLabelFont(132);
    h->GetYaxis()->SetTitleFont(132);
    h->GetYaxis()->SetLabelFont(132);
    h->GetZaxis()->SetTitleFont(132);
    h->GetZaxis()->SetLabelFont(132);
    h->GetXaxis()->SetTitleSize(0.055);
    h->GetXaxis()->SetLabelSize(0.045);
    h->GetYaxis()->SetTitleSize(0.055);
    h->GetYaxis()->SetLabelSize(0.045);
    h->GetZaxis()->SetLabelSize(0.045);
    h->GetXaxis()->SetMaxDigits(3);
    h->GetYaxis()->SetMaxDigits(2);
    h->GetZaxis()->SetMaxDigits(3);
    h->GetYaxis()->SetTitleOffset(0.9);
    h->SetContour(99);
    h->SetStats(0);
    if(!palDefault){
      SetUserPalette();
    }
  } // 2D Histogram Style
  
  // --------------------------------------------------------------------------------------------------------------------------------------------------
  
  void SetHistogramStyle3D(TH3D *h, const char *xLabel, const char *yLabel, const char *zLabel){
    h->GetXaxis()->SetTitle(xLabel);
    h->GetYaxis()->SetTitle(yLabel);
    h->GetZaxis()->SetTitle(zLabel);
    h->GetXaxis()->SetTitleFont(132);
    h->GetXaxis()->SetLabelFont(132);
    h->GetYaxis()->SetTitleFont(132);
    h->GetYaxis()->SetLabelFont(132);
    h->GetZaxis()->SetTitleFont(132);
    h->GetZaxis()->SetLabelFont(132);
    h->GetXaxis()->SetTitleSize(0.045);
    h->GetXaxis()->SetLabelSize(0.035);
    h->GetYaxis()->SetTitleSize(0.045);
    h->GetYaxis()->SetLabelSize(0.035);
    h->GetZaxis()->SetTitleSize(0.045);
    h->GetZaxis()->SetLabelSize(0.035);
    h->GetXaxis()->SetMaxDigits(3);
    h->GetYaxis()->SetMaxDigits(3);
    h->GetZaxis()->SetMaxDigits(3);
    h->SetContour(99);
    h->SetStats(0);
  } // 2D Histogram Style
  
  // --------------------------------------------------------------------------------------------------------------------------------------------------
  
  void FormatLatex(const double &x, const double &y, const char *s, double t, int a){
    // Setup the Latex object
    TLatex l;
    l.SetTextAlign(a); // Align at bottom
    l.SetTextSize(t); 
    l.SetTextFont(132);
    l.DrawLatex(x,y,s);
  }

  void FormatLatexNDC(const double &x, const double &y, const char *s, double t, int a){
    // Setup the Latex object
    TLatex l;
    l.SetTextAlign(a); // Align at bottom
    l.SetTextSize(t); 
    l.SetTextFont(132);
    l.DrawLatexNDC(x,y,s);
  }

  // --------------------------------------------------------------------------------------------------------------------------------------------------

  void FormatStats(TH1D *h, int o, int f, int t){

    // Firstly, turn the stats on for this histogram
    h->SetStats(1);
    gStyle->SetOptStat(o);
    gStyle->SetOptFit(f);

    TPaveStats *st = static_cast<TPaveStats*>(h->FindObject("stats"));
    st->SetBorderSize(0);
    st->SetFillStyle(0);
    st->SetTextFont(t);
    st->SetTextSize(0.035);
    st->SetX1NDC(0.55);
    st->SetY1NDC(0.58);
    st->SetX2NDC(0.92);
    st->SetY2NDC(0.93);
  }
  
  // --------------------------------------------------------------------------------------------------------------------------------------------------

  void WriteStatsToTeX(ofstream   &file,
                       const int  &nFiles,
                       const std::vector<std::string> &contents,
                       const std::vector<unsigned int> &rates,
                       const double &denom,
                       const std::string &denLab){
 
    // Calculate the approximate number of days from the number of files
    // nFiles * 500 events per file / 14101 events per day
    double nDays = (nFiles*500.)/14118.;
    // Start by writing the first few lines of the tex file
    file << "\\begin{document} " << std::endl;
    file << "  \\thispagestyle{empty}" << std::endl;
    file << "  \\renewcommand{\\arraystretch}{1.2}" << std::endl;

    // Setup the table
    file << "  \\begin{table}[h!]" << std::endl;
    file << "    \\centering" << std::endl;
    file << "    \\begin{tabular}{ m{4cm} * {2}{ >{\\centering\\arraybackslash}m{4cm} } }" << std::endl;
    file << "      \\toprule" << std::endl;
    file << "      Statistic & Rate / " << std::setprecision(4) << nDays << " Days & \\% "+denLab+" \\\\" << std::endl;
    file << "      \\midrule" << std::endl;

    for(unsigned int nRate = 0; nRate < rates.size(); ++nRate){
      std::string str = contents.at(nRate);
      unsigned int val = rates.at(nRate);
      if(nRate == 0){
        file << "      " << str << " & " << "\\num{" << std::setprecision(4) << val << "} & - \\\\" << std::endl;
        file << "      \\midrule" << std::endl;
      }
      else
        file << "      " <<  str << " & " << "\\num{" << std::setprecision(4) << val << "} & " << std::setprecision(5) << (val/denom)*100 << "~\\%  \\\\" << std::endl;
    }
    file << "      \\bottomrule" << std::endl;
    file << "    \\end{tabular}" << std::endl;
    file << "  \\end{table}" << std::endl;
    file << "\\end{document}" << std::endl;
  }

  // --------------------------------------------------------------------------------------------------------------------------------------------------

  void SetLogX(TH1* h){
    TAxis* axis = h->GetXaxis();

    double start = TMath::Log10(axis->GetXmin());
    double stop = TMath::Log10(axis->GetXmax());
    double range = stop - start;
    int nbins = axis->GetNbins();
    double binwidth = range / nbins;

    double *bins = new double[nbins+1];
    for (int i = 0; i < (nbins+1); ++i) {
      bins[i] = TMath::Power(10, start + i*binwidth);
    }
    axis->Set(nbins, bins);
    delete[] bins;
  } // Set Log X
  
  // --------------------------------------------------------------------------------------------------------------------------------------------------

  void SetLogY(TH2* h){
    TAxis* axis = h->GetYaxis();

    double start = TMath::Log10(axis->GetXmin());
    double stop = TMath::Log10(axis->GetXmax());
    double range = stop - start;
    int nbins = axis->GetNbins();
    double binwidth = range / nbins;

    double *bins = new double[nbins+1];
    for (int i = 0; i < (nbins+1); ++i) {
      bins[i] = TMath::Power(10, start + i*binwidth);
    }
    axis->Set(nbins, bins);
    delete[] bins;
  } // Set Log X
  
  // --------------------------------------------------------------------------------------------------------------------------------------------------

  double GetPeakBinCentre(TH1 *h){
    // Get the bin containing the maximum content
    int bin = h->GetMaximumBin();
    
    // Calculated this from the low edge and the width in case of log scales
    double low   = h->GetBinLowEdge(bin);
    double width = h->GetBinWidth(bin);

    // Now calculate and return the centre of this bin
    return low+(width/2.);
  }
  
} // calib
