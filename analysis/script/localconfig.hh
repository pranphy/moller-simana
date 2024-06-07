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
            // even though NOQTZ doesn't appear after this, none of these have quartz tiles.
            {"IR4256A606B80", "beam-4IR256AIR606BIR80-202304251138"},
            {"IR4256A556B80-def", "beam-dmap-4IR256AIR556BIR80-202304251726"},
            {"IR4256A606B90-def", "beam-dmap-4IR256AIR606BIR90-202304261222"},
            {"IR4256A606B80-def", "beam-dmap-4IR256AIR606BIR80-202304261720"},
            {"IR4256A556B80-off", "beam-dmap-offset-mm-NOQTZ-4IR256AIR556BIR80-202304291422"}, // with 1mm offset 1 0 -7500 mm /remoll/evgen/beam/origin
            {"IR4256A556B80-bll", "beam-dmap-bp-4IR256AIR556BIR80-202305021158"}, // same configuration as before no offset default map and with bellyplates taper angle 0.78 and 1.23 degree on 6A and 6B
            {"IR4256A556B80-0.81.2", "beam-dmap-bp-255580-0.81.2-02"},               // no offset default map and with bellyplates taper angle 0.80 and 1.20 degree on 6A and 6B no quarts off course.
            {"IR4256A556B80-0.81.2-03", "beam-dmap-bp-255580-0.81.2-03"},            // Same geometry just reconfirming
            {"IR4256A556B80-0.81.2-cls6a", "beam-dmap-cls6a-255580-0.81.2"}, // Ok now same everything, but just closed the gap in Collimator 6A with 4mm overlap.
            {"IR4266A556B80-0.81.2-srt6a-def", "beam-dmap-srt6a-265580-0.81.2"}, //  Shortened 6A by 0.25 inches. Default field and as the previous, closed and 4mm overlap.
            {"IR4266A556B80-0.81.2-srt6a-rasym", "beam-rasym-srt6a-265580-0.81.2"}, // Same thing, but with real assymetric field.
            {"IR4266A556B80-0.81.2-dosrt", "beam-dmap-offset-srt6a-265580-0.81.2"}, // Now is default map, with 1mm offset and shortened/closed 6A. of course 26mm 55.31mm 80.00mm. And 0.8 and 1.2 degree.
            {"IR4266A556B80-0.81.2-aosrt", "beam-rasym-offset-srt6a-265580-0.81.2"}, // Assymetric map, with 1mm offset and shortened/closed 6A. of course 26mm 55.31mm and 80mm, with 0.8 and 1.2 degree.
        };
        std::string filename(int ir4,int ir6a, int ir6b,std::string field=""){
            std::string id = "IR4"+_s(ir4)+"6A"+_s(ir6a)+"6B"+_s(ir6b)+field;
            return "sim/output/"+files[id]+"/primary/skim/merged/events-at-least-hitting-noepm.root";
        }
    } // sixab::
}
