#pragma once
#include <map>
#include <string>

std::string _s(int a){
    return std::to_string(a);
}

namespace cfg{
    std::map<std::string,std::string> local = {
        // Found out that there wer positrons missing in the selected_MD_hits file I did, so redid with positrons.
        {"ryansim_develoop_skim", "/mnt/stg/data/remoll/sim/output/ryan/develop_background_study_100kEv-positrons/filelist-small-with-positrons.txt"},
        // Simulation from ryan with new collimator
        {"ryansim_new_coll","/mnt/stg/data/remoll/sim/output/ryan/selected-Ring5-100Mbeamevents.root"},
        // With the lintel present and with moller generator a sample file.
        {"yes_lintel_moller_one","/home/pranphy/simana/sim/output/yes-lintel-20230206-155101/primary/yes-lintel-20230206-155101-100000-0005.root"},
        // Added kryptonite before Coll2 and after MD and NO[6a and 6b] with the updated geometry commit: 1502772a21df
        {"kryptonite_test_col1det28","/mnt/stg/data/remoll/sim/output/add-kryptonite/single/add-kryptonite-20230308-1148-1000-001.root"}
    };

    namespace sixab{
        std::map<std::string,std::string> files{
            {"IR4406A656B95", "beam-NOQTZ-6AIR656BIR95IA2.62e-2-202304111455"},
            {"IR4406A606B90", "beam-NOQTZ-6AIR606BIR90IA2.62e-2-202304141213"},
            {"IR4256A606B90", "beam-NOQTZ-4IR256AIR606BIR90IA2.62e-2-202304191026"},
            {"IR4406A556B80", "beam-NOQTZ-6AIR556BIR80-202304201118"},
            {"IR4256A556B80", "beam-NOQTZ-4IR256AIR556BIR80-202304211410"},
        };
        std::string filename(int ir4,int ir6a, int ir6b){
            std::string id = "IR4"+_s(ir4)+"6A"+_s(ir6a)+"6B"+_s(ir6b);
            return "sim/output/"+files[id]+"/primary/skim/merged/events-at-least-hitting-noepm.root";
        }
    } // sixab::
}
