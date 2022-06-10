//
// Author : K.v.Sturm
// Date   : 2022-06-09
// Usage  : ./extraction (from the top folder)
//

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "TFile.h"
#include "TH1D.h"

#include "json.hpp"

using namespace std;
using nlohmann::json;

map<int,string> get_detmap(string phase, string det);

int main( int argc, char* argv[] ) {

  // loop over ph2(enrBEGe,enrCoax) / ph2p(enrBEGe,enrCoax,invCoax)
  vector<string> v_p = {"ph2", "ph2p"};
  vector<string> v_d = {"enrBEGe", "enrCoax", "invCoax"};
  vector<string> v_s = {"central", "m1s-corr", "m1s-uncorr", "p1s-corr", "p1s-uncorr"};

  string isotope = argv[1];

  for (auto phase : v_p) {
    cout << "Loop " << phase << endl;

    // created the json stuff one for each phase
    json j_kr85;

    for (auto stuff : v_s) {
      cout << "Loop " << stuff << endl;

      string fname;
      if      (isotope == "Kr85") fname = phase + "/pdf-" + phase + "-Kr85-decay-start-" + stuff + ".root";
      else if (isotope == "Ar36") fname = phase + "/pdf-" + phase + "-Ar36-" + stuff + ".root";
      TFile * f = new TFile(fname.c_str());

      for (auto ds : v_d) {
        // get the detector map
        map<int,string> det_map = get_detmap(phase, ds);
        if (det_map.empty()) continue;

        for (const auto &ch_det : det_map) {
          string hname = "raw/M1_ch" + to_string(ch_det.first);

          f->cd();
          TH1D* h = (TH1D*)gDirectory->Get(hname.c_str());
          int cs;
          if      (isotope == "Kr85") cs = h->Integral(509,519) - h->Integral(504,509);
          else if (isotope == "Ar36") cs = h->Integral(425,435) - h->Integral(420,425);
          if      (stuff == "central")    j_kr85[ds][ch_det.second]["value"]       = cs;
          else if (stuff == "m1s-corr")   j_kr85[ds][ch_det.second]["corr"]["m1s"] = cs;
          else if (stuff == "p1s-corr")   j_kr85[ds][ch_det.second]["corr"]["p1s"] = cs;
          else if (stuff == "m1s-uncorr") j_kr85[ds][ch_det.second]["uncorr"]["m1s"] = cs;
          else if (stuff == "p1s-uncorr") j_kr85[ds][ch_det.second]["uncorr"]["p1s"] = cs;
          delete h;
        }
      }

      f->Close();
    }
    // write all the stuff in the json file
    ofstream j_out(phase + "_" + isotope + ".json");
    j_out << j_kr85.dump(4);
    j_out.close();
  }

  cout << "DONE" << endl;

  return 0;
}

map<int,string> get_detmap(string phase, string det) {

  map<int,string> det_map;

  if (phase == "ph2" && det == "enrBEGe") {
    det_map[0] = "GD91A";
    det_map[1] = "GD35B";
    det_map[2] = "GD02B";
    det_map[3] = "GD00B";
    det_map[4] = "GD61A";
    det_map[5] = "GD89B";
    det_map[6] = "GD02D";
    det_map[7] = "GD91C";
    det_map[11] = "GD02A";
    det_map[12] = "GD32B";
    det_map[13] = "GD32A";
    det_map[14] = "GD32C";
    det_map[15] = "GD89C";
    det_map[16] = "GD61C";
    det_map[17] = "GD76B";
    det_map[18] = "GD00C";
    det_map[19] = "GD35C";
    det_map[20] = "GD76C";
    det_map[21] = "GD89D";
    det_map[22] = "GD00D";
    det_map[23] = "GD79C";
    det_map[24] = "GD35A";
    det_map[25] = "GD91B";
    det_map[26] = "GD61B";
    det_map[30] = "GD00A";
    det_map[31] = "GD02C";
    det_map[32] = "GD79B";
    det_map[33] = "GD91D";
    det_map[34] = "GD32D";
    det_map[35] = "GD89A";
  }
  else if (phase == "ph2" && det == "enrCoax") {
    det_map[8] = "ANG5";
    det_map[9] = "RG1";
    det_map[10] = "ANG3";
    det_map[27] = "ANG2";
    det_map[28] = "RG2";
    det_map[29] = "ANG4";
    det_map[36] = "ANG1";
  }
  else if (phase == "ph2p" && det == "enrBEGe") {
    det_map[0] = "GD91A";
    det_map[1] = "GD35B";
    det_map[2] = "GD02B";
    det_map[3] = "GD00B";
    det_map[4] = "GD61A";
    det_map[5] = "GD89B";
    det_map[6] = "GD02D";
    det_map[7] = "GD91C";
    det_map[11] = "GD02A";
    det_map[12] = "GD32B";
    det_map[13] = "GD32A";
    det_map[14] = "GD32C";
    det_map[15] = "GD89C";
    det_map[16] = "GD61C";
    det_map[17] = "GD76B";
    det_map[18] = "GD00C";
    det_map[19] = "GD35C";
    det_map[20] = "GD76C";
    det_map[21] = "GD89D";
    det_map[22] = "GD00D";
    det_map[23] = "GD79C";
    det_map[24] = "GD35A";
    det_map[25] = "GD91B";
    det_map[26] = "GD61B";
    det_map[30] = "GD00A";
    det_map[31] = "GD02C";
    det_map[32] = "GD79B";
    det_map[33] = "GD91D";
    det_map[34] = "GD32D";
    det_map[35] = "GD89A";
  }
  else if (phase == "ph2p" && det == "enrCoax") {
    det_map[8] = "ANG5";
    det_map[9] = "RG1";
    det_map[10] = "ANG3";
    det_map[27] = "ANG2";
    det_map[28] = "RG2";
    det_map[29] = "ANG4";
  }
  else if (phase == "ph2p" && det == "invCoax") {
    det_map[36] = "IC48B";
    det_map[37] = "IC50B";
    det_map[38] = "IC48A";
    det_map[39] = "IC50A";
    det_map[40] = "IC74A";
  }
  else {
    cout << "Phase/Dataset combo " << phase << "/" << det << " undefined. Map Empty." << endl;
  }

  return det_map;
}
