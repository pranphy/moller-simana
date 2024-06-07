#include "misc_utils.hh"

std::string opfile="./asset/files/root/bellows4_all_UPDATE.root";
std::vector<double> bellows4 = {12210,12670,680,820};
std::vector<double> smallbox = {12606,12650,682,690};

void make_sec_plots(std::string filelist){
    ROOT::EnableImplicitMT();
    ROOT::RDataFrame df("T",utl::readlines(filelist));

    auto vzvrhist = new TH2D("vzvrhist","The vertex of those that hit Ring 0; vz[mm]; vr[mm]",300, 12220,12670,300,680,820);
    auto smbox = new TH2D("smbox","The vertex of those that hit Ring 0; vz[mm]; vr[mm]",300, 12605,12629,300,681,691);
    auto vzhist = new TH1D("vzhist","The vertex of those that hit Ring 0; vz[mm]",300, 12220,12670);
    auto phihist = new TH1D("phihist","The secondary hit at Bellows4; phi[degree]; count/5 degree",72, -180,180);
    auto vphihist = new TH1D("phihist","The phi dist of vx,vy from bellows4 to ring0; phi[degree]; count/5 degree",72, -180,180);
    auto fill_hist = [&](RemollHit hit)->void {
        double vr = std::hypot(hit.vx,hit.vy);
        if(utl::cut::ring0_epm_E1(hit) and utl::is_inside(hit.vz,vr, bellows4)) vzvrhist->Fill(hit.vz,vr);
        if(utl::cut::ring0_epm_E1(hit) and utl::is_inside(hit.vz,vr, smallbox)) smbox->Fill(hit.vz,vr);
        if(utl::cut::epm(hit) and hit.det == 911 ) phihist->Fill(utl::atan_deg(hit.y,hit.x));
        if(utl::cut::ring0_epm_E1(hit) and utl::is_inside(hit.vz,vr,bellows4) ) vphihist->Fill(utl::atan_deg(hit.vy,hit.vx));
        if(utl::cut::ring0_epm_E1(hit) and utl::is_inside(hit.vz,vr,bellows4) ) vzhist->Fill(hit.vz);
    };
    loop_tree(df,fill_hist);
    //vphihist->Draw("hist");
    //phihist->Draw("hist");

    //vzvrhist->Draw("colz");  smbox->Draw("colz"); utl::draw_quad(smallbox);
    //utl::show_text(" z = 12606 - 12628 ; r = 682 690 mm",12580,692);

    utl::save(vzvrhist,"ring0-from-bellows4","bellows4_cfg02/plot",opfile);
    utl::save(smbox,"ring0-from-smallbox","bellows4_cfg02/plot",opfile);
    utl::save(phihist,"hit-at-bellows4-epm","bellows4_cfg02/plot",opfile);
    utl::save(vphihist,"ring0-from-bellows4-phidist","bellows4_cfg02/plot",opfile);
    utl::save(vzhist,"ring0-from-bellows4-vzdist","bellows4_cfg02/plot",opfile);
    //utl::moller_fraction(16878, 380e5,7233,100e6)*100
    //utl::moller_fraction(55099, 380e5,7233,100e6)/3300
    //utl::moller_fraction(4863, 380e5,7233,100e6)/3300
}

void bellows_4(std::string filelist=""){
    if (filelist == "")
        filelist="/mnt/stg/data/remoll/sim/output/beam-bellows4-cfg01/secondary/911/default/filelist.txt";

    std::cout<<" Analyzing file"<<filelist<<std::endl;
    make_sec_plots(filelist);
}
