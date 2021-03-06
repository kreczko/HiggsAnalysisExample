void correctBkgModel(bool updateWorkspace=false) {

  TString option;
  option = updateWorkspace ? "UPDATE" : "READ";

  TFile *workspace = TFile::Open("/afs/cern.ch/user/f/futyand/scratch1/mva_ucsd/CMS-HGG_bdt_MITeleVeto_15_01_2012.root",option);
  TFile *f_bias = TFile::Open("/afs/cern.ch/user/f/futyand/scratch1/mva_ucsd/BkgBias_15Jan/BkgBias.root");

  //TFile *workspace_temp;
  //if (!updateWorkspace) workspace_temp = TFile::Open("CMS-HGG_biascorr.root","RECREATE");

  TString boost_str[2] = {"grad","ada"};
  TString mass_str_part[2] = {".0",".5"};
  float mass_part[2] = {.0,.5};

  float sidebandWidth=0.02;
  float signalRegionWidth=0.07;

  for (int imass=115; imass<151; imass++){
    for (int j=0; j<2; j++) {
      if (j==1 && imass==150) continue;

      TString allmass_str;
      allmass_str+=imass;
      allmass_str+=mass_str_part[j];
      TString binningmass_str = getBinningMass(float(imass) + mass_part[j]);

      for (int boost=0; boost<2; boost++) {

	graph_data = (TGraph*)(f_bias->Get("tgraph_biasslopes_data_"+boost_str[boost]+"_"+binningmass_str))->Clone();
	graph_mc = (TGraph*)(f_bias->Get("tgraph_biasslopes_mc_"+boost_str[boost]+"_"+binningmass_str))->Clone();

	TH1* hist_bkgModel = (TH1*)(workspace->Get("th1f_bkg_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();

	TH1* hist_data_sb[7];
	hist_data_sb[0] = (TH1*)(workspace->Get("th1f_bkg_3low_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	hist_data_sb[1] = (TH1*)(workspace->Get("th1f_bkg_2low_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	hist_data_sb[2] = (TH1*)(workspace->Get("th1f_bkg_1low_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	hist_data_sb[3] = (TH1*)(workspace->Get("th1f_data_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	hist_data_sb[4] = (TH1*)(workspace->Get("th1f_bkg_1high_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	hist_data_sb[5] = (TH1*)(workspace->Get("th1f_bkg_2high_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	hist_data_sb[6] = (TH1*)(workspace->Get("th1f_bkg_3high_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	TH1* hist_mc_sb[7];
	hist_mc_sb[0] = (TH1*)(workspace->Get("th1f_bkg_mc_3low_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	hist_mc_sb[1] = (TH1*)(workspace->Get("th1f_bkg_mc_2low_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	hist_mc_sb[2] = (TH1*)(workspace->Get("th1f_bkg_mc_1low_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	hist_mc_sb[3] = (TH1*)(workspace->Get("th1f_bkg_mc_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	hist_mc_sb[4] = (TH1*)(workspace->Get("th1f_bkg_mc_1high_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	hist_mc_sb[5] = (TH1*)(workspace->Get("th1f_bkg_mc_2high_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();
	hist_mc_sb[6] = (TH1*)(workspace->Get("th1f_bkg_mc_3high_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone();

	float mass_sb[7];
	float mass_sig = float(imass) + mass_part[j];
	mass_sb[2] = mass_sig*(1-signalRegionWidth)/(1+sidebandWidth);
	mass_sb[1] = mass_sb[2]*(1-sidebandWidth)/(1+sidebandWidth);
	mass_sb[0] = mass_sb[1]*(1-sidebandWidth)/(1+sidebandWidth);
	mass_sb[3] = mass_sig;
	mass_sb[4] = mass_sig*(1+signalRegionWidth)/(1-sidebandWidth);
	mass_sb[5] = mass_sb[4]*(1+sidebandWidth)/(1-sidebandWidth);
	mass_sb[6] = mass_sb[5]*(1+sidebandWidth)/(1-sidebandWidth);

	TH1* hist_data_corrected_sb[7];
	TH1* hist_data_corrected_up_sb[7];
	TH1* hist_data_corrected_down_sb[7];
	TH1* hist_mc_corrected_sb[7];

	for (int isb=0; isb<7; isb++) {
	  if (isb==3) continue;

	  hist_data_corrected_sb[isb] = (TH1*)hist_data_sb[isb]->Clone();
	  hist_data_corrected_up_sb[isb] = (TH1*)hist_data_sb[isb]->Clone();
	  hist_data_corrected_down_sb[isb] = (TH1*)hist_data_sb[isb]->Clone();
	  hist_mc_corrected_sb[isb] = (TH1*)hist_mc_sb[isb]->Clone();

	  float deltam = mass_sb[isb]-mass_sig;

	  for (int ibin=1; ibin<hist_data_sb[isb]->GetNbinsX()+1; ibin++) {

	    float slope = graph_data->Eval(float(ibin)-0.5);
	    float slope_err = graph_data->Eval(float(ibin)-0.5);
	    float corrfac = 1./(1.+(slope*deltam));
	    float corrfac_up = 1./(1.+((slope+slope_err)*deltam));
	    float corrfac_down = 1./(1.+((slope-slope_err)*deltam));
	    hist_data_corrected_sb[isb]->SetBinContent(ibin,hist_data_sb[isb]->GetBinContent(ibin)*corrfac);
	    hist_data_corrected_up_sb[isb]->SetBinContent(ibin,hist_data_sb[isb]->GetBinContent(ibin)*corrfac_up);
	    hist_data_corrected_down_sb[isb]->SetBinContent(ibin,hist_data_sb[isb]->GetBinContent(ibin)*corrfac_down);

	    slope = graph_mc->Eval(float(ibin)-0.5);
	    corrfac = 1./(1.+(slope*deltam));
	    hist_mc_corrected_sb[isb]->SetBinContent(ibin,hist_mc_sb[isb]->GetBinContent(ibin)*corrfac);

	  }

	  hist_data_corrected_sb[isb]->Scale(hist_data_sb[isb]->Integral()/hist_data_corrected_sb[isb]->Integral());
	  hist_data_corrected_up_sb[isb]->Scale(hist_data_sb[isb]->Integral()/hist_data_corrected_up_sb[isb]->Integral());
	  hist_data_corrected_down_sb[isb]->Scale(hist_data_sb[isb]->Integral()/hist_data_corrected_down_sb[isb]->Integral());
	  hist_mc_corrected_sb[isb]->Scale(hist_mc_sb[isb]->Integral()/hist_mc_corrected_sb[isb]->Integral());

	}

	TH1* hist_data_corrected = (TH1*)(workspace->Get("th1f_bkg_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone("th1f_bkg_"+boost_str[boost]+"_"+allmass_str+"_cat0_biascorr");
	TH1* hist_data_corrected_up = (TH1*)(workspace->Get("th1f_bkg_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone("th1f_bkg_"+boost_str[boost]+"_"+allmass_str+"_cat0_biascorr_biasUp01_sigma");
	TH1* hist_data_corrected_down = (TH1*)(workspace->Get("th1f_bkg_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone("th1f_bkg_"+boost_str[boost]+"_"+allmass_str+"_cat0_biascorr_biasDown01_sigma");
	TH1* hist_mc_corrected = (TH1*)(workspace->Get("th1f_bkg_mc_"+boost_str[boost]+"_"+allmass_str+"_cat0"))->Clone("th1f_bkgModel_mc_"+boost_str[boost]+"_"+allmass_str+"_cat0_biascorr");

	hist_data_corrected->Reset();
	hist_data_corrected_up->Reset();
	hist_data_corrected_down->Reset();
	hist_mc_corrected->Reset();

	for (int isb=0; isb<7; isb++) {
	  if (isb!=3) {
	    hist_data_corrected->Add(hist_data_corrected_sb[isb]);
	    hist_data_corrected_up->Add(hist_data_corrected_up_sb[isb]);
	    hist_data_corrected_down->Add(hist_data_corrected_down_sb[isb]);
	    hist_mc_corrected->Add(hist_data_corrected_sb[isb]);
	  }
	}
	hist_data_corrected->Scale(hist_bkgModel->Integral()/hist_data_corrected->Integral());
	hist_data_corrected_up->Scale(hist_bkgModel->Integral()/hist_data_corrected_up->Integral());
	hist_data_corrected_down->Scale(hist_bkgModel->Integral()/hist_data_corrected_down->Integral());

	if (updateWorkspace) {
	  workspace->cd();
	  //if (!updateWorkspace) workspace_temp->cd();
	  hist_data_corrected->Write();
	  hist_data_corrected_up->Write();
	  hist_data_corrected_down->Write();
	  hist_mc_corrected->Write();
	}

      }
    }
  }

}

TString getBinningMass(float mass) {

  if (mass >= 115.0 && mass <= 117.0) return "115";
  if (mass >= 117.5 && mass <= 122.0) return "120";
  if (mass >= 122.5 && mass <= 127.0) return "125";
  if (mass >= 127.5 && mass <= 132.0) return "130";
  if (mass >= 132.5 && mass <= 137.0) return "135";
  if (mass >= 137.5 && mass <= 144.5) return "140";
  if (mass >= 145.0 && mass <= 150.0) return "150";

}
