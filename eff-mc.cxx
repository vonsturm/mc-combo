//
// Author : K.v.Sturm
// Date   : 9th June 2022
// Note   : MC combination of uncertainties fully correlated and uncorrelated
// Usage  : ./eff-mc -d <datasets> -s <stat>
// Inputs : <datasets> : json file containing the datasets
//          <stat> : statistics to be generated
//

// C/c++
#include <cstdio>
#include <fstream>

// Cern Root
#include "TH1.h"
#include "TFile.h"
#include "TRandom3.h"

// other stuff
#include "json.hpp"
#include "./args-reader/args_reader.hpp"

using nlohmann::json;
using namespace args_reader;

void usage() {
  std::cout << "\nMC combination of correlated and uncorrelated uncertainties\n\n";
  std::cout << "Usage: ./eff-mc <options>\n\n";
  std::cout << "Options:\n\n";
  std::cout << "mandatory:\n\n";
  std::cout << "  -d (--datasets) <file.json> : .json file containing datasets and values\n";
  std::cout << "optional:\n\n";
  std::cout << "  -h (--help)        : Print this user manual\n";
  std::cout << "  -uu <m1s,p1s,both> : minus/plus 1sigma uncorr. uncertainty \n";
  std::cout << "  -cu <m1s,p1s,both> : minus/plus 1sigma corr. uncertainty \n";
  std::cout << "  -o (--output)      : Save histograms to this file (default out.root)\n";
  std::cout << "  -v (--verbose)     : More output\n";
  std::cout << "  -s (--stat)        : statistics to generate (default 1e6)\n\n";
}

int main(int argc, char* argv[]) {

  std::vector<std::string> args(argc);
  for (int i = 0; i < argc; ++i) args.at(i) = argv[i];

  bool help = fetch_arg(args, "-h") or
              fetch_arg(args, "--help");

  if (help) { usage(); return 0; }

  bool verbose = fetch_arg(args, "-v") or
                 fetch_arg(args, "--verbose");

  std::string datasets;
  fetch_arg(args, "-d", datasets) or
  fetch_arg(args, "--datasets", datasets);

  int stat = 1e6;
  fetch_arg(args, "-s", stat) or
  fetch_arg(args, "--stat", stat);

  std::string outfile = "out.root";
  fetch_arg(args, "-o", outfile) or
  fetch_arg(args, "--output", outfile);

  std::string uu = "both", cu = "both";
  fetch_arg(args, "-uu", uu);
  fetch_arg(args, "-cu", cu);

  // read json input
  json j_datasets;
  if (datasets != "") {
    std::ifstream f_ds(datasets);
    if (f_ds.is_open()) f_ds >> j_datasets;
    f_ds.close();
  }
  else { usage(); return 1; }

  // open output file
  TFile * of = new TFile(outfile.c_str(), "RECREATE");

  // loop over datasets
  for (auto & ds : j_datasets.items()) {

    std::cout << "Found dataset " << ds.key() << " starting MC evaluation with " << stat << " events." << std::endl;

    // store all values in a vector first
    int n = 4;
    if      (cu == "both" and uu == "both") n = 4;
    else if (cu == "both" or  uu == "both") n = 2;
    else                                    n = 1;
    std::vector<double> v_ds(stat*n,0);

    // loop over detectors in datasets
    for(int i = 0; i < stat; i++) {
      // TODO: Add progress bar
      static double sum_value_m1s_m1s = 0;
      static double sum_value_p1s_p1s = 0;
      static double sum_value_p1s_m1s = 0;
      static double sum_value_m1s_p1s = 0;
      // generate random gauss for correlated unc. 1x for dataset
      TRandom3 rand; rand.SetSeed(0);
      double r_corr = rand.Gaus();
      for (auto & det : ds.value().items()) {
        // generate random gauss for uncorrelated unc. 1x for detector
        if (verbose) std::cout << det.key() << std::endl;

        double r_uncorr = rand.Gaus();
        double val_c = det.value()["value"].get<double>();
        double val_cu_m1s = r_corr * abs(val_c - det.value()["corr"]["m1s"].get<double>());
        double val_cu_p1s = r_corr * abs(val_c - det.value()["corr"]["p1s"].get<double>());
        double val_uu_m1s = r_uncorr * abs(val_c - det.value()["uncorr"]["m1s"].get<double>());
        double val_uu_p1s = r_uncorr * abs(val_c - det.value()["uncorr"]["p1s"].get<double>());
        sum_value_m1s_m1s += val_c + val_cu_m1s + val_uu_m1s;
        sum_value_p1s_p1s += val_c + val_cu_p1s + val_uu_p1s;
        sum_value_p1s_m1s += val_c + val_cu_p1s + val_uu_m1s;
        sum_value_m1s_p1s += val_c + val_cu_m1s + val_uu_p1s;
      }
      int j = 0;
      if      ((cu == "both" or cu == "m1s") and (uu == "both" or uu == "m1s")) {
        v_ds.at(i*n + j++) = sum_value_m1s_m1s;
      }
      if ((cu == "both" or cu == "p1s") and (uu == "both" or uu == "p1s")) {
        v_ds.at(i*n + j++) = sum_value_p1s_p1s;
      }
      if ((cu == "both" or cu == "p1s") and (uu == "both" or uu == "m1s")) {
        v_ds.at(i*n + j++) = sum_value_p1s_m1s;
      }
      if ((cu == "both" or cu == "m1s") and (uu == "both" or uu == "p1s")) {
        v_ds.at(i*n + j) = sum_value_m1s_p1s;
      }
      sum_value_m1s_m1s = 0; sum_value_p1s_p1s = 0; sum_value_p1s_m1s = 0; sum_value_m1s_p1s = 0;
    }

    // create the histogram
    std::string histname = "mc_hist_" + ds.key();
    std::string histtitle = ds.key() + "; C_s; counts";
    double min = *min_element(v_ds.begin(), v_ds.end());
    double max = *max_element(v_ds.begin(), v_ds.end());
    TH1D * hist = new TH1D(histname.c_str(), histtitle.c_str(), 1000, min - (max-min)/10., max + (max-min)/10.);

    // histogram stuff
    for (auto el : v_ds) hist->Fill(el);

    // central value and RMS
    double val = hist->GetBinCenter(hist->GetMaximumBin());
    double rms = hist->GetRMS();

    std::cout << "Mean " << val << " rms " << rms << std::endl;

    // save to file and output
    hist->Write();
  }

  of->Close();
  std::cout << "DONE" << std::endl;

  return 0;
}
