# MC combo

Monte Carlo combination of fully correlated and uncorrelated uncertainties of
HPGe detection efficiencies.

## Extraction

After PDF production the relevant quantities need to be extracted from the
spectra and for further processing to be stored in a `.json` file of specific
format. The `extraction` program is doing this job for you. Implemented are

 * `Kr85` : Signal Window 509-519 keV Background Window 504-509 keV
 * `Ar36` : Signal Window 425-435 keV Background Window 420-425 keV

If you want to implement a different quantity or a different isotope you can
easily add it to the code.

The output .json file is in the following format (see also `template.json`). The
top level denotes the `dataset` and the second level the `detectors` it
contains. For each detector the central `value` and `&pm;$\sigma$` for
correlated and uncorrelated uncertainties are stored.

```json
{
  "enrBEGe" : {
    "GD61A" : {
      "value" : 500,
      "corr" : { "m1s" : 510, "p1s" : 400 },
      "uncorr" : { "m1s" : 520, "p1s" : 450 }
    },
    "GD91B" : {
      "value" : 503,
      "corr" : { "m1s" : 513, "p1s" : 499 },
      "uncorr" : { "m1s" : 530, "p1s" : 489 }
    }
  },
  "enrCoax" : {
    "RG1" : {
      "value" : 500,
      "corr" : { "m1s" : 501, "p1s" : 490 },
      "uncorr" : { "m1s" : 580, "p1s" : 480 }
    },
    "ANG1" : {
      "value" : 520,
      "corr" : { "m1s" : 550, "p1s" : 450 },
      "uncorr" : { "m1s" : 590, "p1s" : 400 }
    }
  }
}
```

## mc-combo

Based on the .json file from the previous step you can now combine uncertainties
where correlated uncertainties are assumed to be fully correlated between all
detectors and uncorrelated uncertaonties fully uncorrelated.

```
MC combination of correlated and uncorrelated uncertainties

Usage: ./mc-combo -d <ds.json> <options>

Options:

mandatory:

  -d (--datasets) <ds.json> : .json file containing datasets and values

optional:

  -h (--help)        : Print this user manual
  -uu <m1s,p1s,both> : ± 1σ uncorr. uncertainty
  -cu <m1s,p1s,both> : ± 1σ corr. uncertainty
  -o (--output)      : Save histograms to this file (default out.root)
  -v (--verbose)     : More output
  -s (--stat)        : statistics to generate (default 1e6)
```

Values are sampled according to the chosen statistics and stored in a histogram.
You can choose which uncertainty combination to use. Default is all four
combinations ± 1σ uncorr. X ± 1σ corr. The bin center of the maximum-bin is
printed on screen as the most probable value while the standard deviation is
given as uncertainty. The histograms for all datasets are stored in a .root file
from which you can easily recover the values.
