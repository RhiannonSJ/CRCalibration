/*
 * @brief Helper functions for general analysis
 *
 */
#ifndef ANALYSISHELPERS_H
#define ANALYSISHELPERS_H

#include "../Setup/Setup.h"

namespace calib{

  /**
   * @brief Check if the track start point is above the end point and flip if not
   *
   * @param start  Track start point
   * @param end    Track end point
   *
   */
  void CheckAndFlip(TVector3 &start, 
                           TVector3 &end);
  
  /**
   * @brief Get the true energy associated to the current reconstructed track
   *
   * @param iTrk  Iterator of the current reco track
   * @param nGeant  number of G4 entries
   * @param evt  anatree event objects
   * @param bP  bestPlane iterator
   *
   * @return true energy
   */
  double GetTrueEnergyAssoc(const int &iTrk, 
                                   const int &nGeant, 
                                   const anatree *evt, 
                                   const int &bP);
      
  /**
   * @brief Check if the current track ID is in the good GEANT list
   *
   * @param trueID true ID of the current reco track
   * @param goodG4 vector of G4 IDs which pass the chosen cuts
   *
   * @return true if the IDs match
   */
  bool CheckTrueIDAssoc(const int &trueID, 
                               const std::vector<int> &goodG4);

  /**
   * @brief Fill the vectors regarding the number of hits on each plane
   *
   * @param id  Current G4 track id
   * @param nHits  Number of hits to check
   * @param evt  anatree event objects
   * @param hitAssoc Hit association to plane vector to fill
   * @param hitsOnPlane Number of hits on each plane vector to fill
   *
   */
  void GetNHitsOnPlane(const int &id, 
                              const int &nHits, 
                              const anatree *evt,
                              std::vector<std::vector<bool>> &hitAssoc, 
                              std::vector<int> &hitsOnPlane);
  

  /**
   * @brief Extract the MPV and FWHM from the TF1 function
   *
   * @param h  TH1 to evaluate
   * @param f  TF1 to evaluate
   * @param fwhm  Langau full width half max
   * @param fitMaxX  Function maximum value (on the x axis)
   *
   */
  void GetFWHMFromTF1(TH1 *h, 
                             TF1 *f, 
                             double &fwhm, 
                             double &fitMaxX);
  
  /**
   *
   * @brief Get the coefficient of Gsigma which translates MP to MPV
   *
   * @param mpv   MPV, central value of the maximum bin in the histogram
   * @param mp    MP, Landau MP given by the fit
   * @param gsig  GSigma, Gaussian sigma given by the fit
   * @param psi   Psi, coefficient to apply to GSigma in the translation
   *
   */
  void GetCoefficient(const double &mpv, 
                             const double &mp, 
                             const double &gsig, 
                             double &psi);

  /**
   *
   * @brief Get the uncertainty on MPV from MP and GSigma
   *
   * @param mpv         MPV, central value of the maximum bin in the histogram
   * @param mp          MP, Landau MP given by the fit
   * @param mp_error    MP uncertainty, Landau MP error given by the fit
   * @param gsig        GSigma, Gaussian sigma given by the fit
   * @param gsig_error  GSigma uncertainty, Gaussian sigma error given by the fit
   * @param psi         Psi, coefficient to apply to GSigma in the translation
   * @param mpv_error   MPV error calculated with the fit parameters and errors
   *
   * @return true if possible, false if psi incorrectly calculated
   *
   */
  bool GetMPVUncertainty(const double &mpv, 
                                const double &mp, 
                                const double &mp_error, 
                                const double &gsig, 
                                const double &gsig_error, 
                                const double &psi,
                                double &mpv_error);

} // calib
#endif