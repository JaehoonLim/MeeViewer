#include "include/SimpleViewer.h"
//#include "include/Cube.h" // Cube Test
#include "include/Wave.h" // Wave Test


/*
// Input/Output File
string input_path = "Input";
string input_file = "Cube.mee";
string output_path = "Output";

// Video Trim
int f_start = 0;
int f_end = 0; // NoCut:0 (33 FPS)

// Video Crop
int x_min = 140;
int x_max = 300; // NoCut:0 / MAX:320 pixel
int y_min = 100;
int y_max = 160; // NoCut:0 / MAX:240 pixel
int d_min = 800;
int d_max = 1400; // NoCut:0 / MAX:6500 mm

// Video Zoom (Crop Area)
bool zoom_all = false; // X & Y & D
bool zoom_x = false;
bool zoom_y = false;
bool zoom_d = false;

// Object Size Cut
int obj_x = 50; // NoCut:0 / pixel
int obj_y = 50; // NoCut:0 / pixel
int obj_z = 150; // NoCut:0 / mm

// Draw Object Position Mean Line
bool draw_mean_line = true;

// Draw Object Position Graph
bool draw_mean_graph = true;
*/


// Main Function Start
void MeeViewer(){

    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(0);
    gStyle->SetMarkerSize(0.8);
    gStyle->SetPalette(1);

    m_pLoadFile = fopen((input_path+"/"+input_file).c_str(),"rb");
    input_file.erase(input_file.end()-4,input_file.end());
    fread(&m_stFrameHeader, sizeof(m_stFrameHeader), 1, m_pLoadFile);
    fseek(m_pLoadFile, sizeof(m_stFrameHeader), SEEK_SET);

    for(int i=0; i<2; ++i){//0:AMPLITUDE, 1:Depth
	m_pCameraBuf[i]	= (unsigned short *)malloc(320*240*sizeof(unsigned short));
	memset(m_pCameraBuf[i], 0, 320*240*sizeof(unsigned short));

    }
    m_pRGBCameraBuf	= (unsigned char *)malloc(640*480*3*sizeof(unsigned char));
    memset(m_pRGBCameraBuf, 0, 640*480*3*sizeof(unsigned char));

    double x_start = 0.0;
    double x_end = 320;
    double y_start = 0.0;
    double y_end = 240;
    double d_start = 0.0;
    double d_end = 3000.0;

    if(f_end==0) f_end = m_stFrameHeader.nFrames;
    if(x_max==0) x_max = x_end;
    if(y_max==0) y_max = y_end;
    if(d_max==0) d_max = d_end;

    if(zoom_all || zoom_x){
        x_start = (double)x_min; 
        x_end   = (double)x_max; 
    }
    if(zoom_all || zoom_y){
        y_start = (double)y_min; 
        y_end   = (double)y_max; 
    }
    if(zoom_all || zoom_d){
        d_start = (double)d_min; 
        d_end   = (double)d_max; 
    }

    int x_bin = int((x_end-x_start)/10);
    int y_bin = int((y_end-y_start)/10);
    int d_bin = int((d_end-d_start)/100);

    // Output Picture Size
    int output_pic_width = 1000; // pixel
    int output_pic_height = 800; // pixel
    if(draw_mean_graph) output_pic_width = 2000;

    TCanvas* c_Meere = new TCanvas("c_Meere","CubeEye Meere File Viewer",output_pic_width,output_pic_height);
    TH3D* h_3d =  new TH3D("h_3d","3D;Width [pixel];Depth [mm];Height [pixel]",x_bin,x_start,x_end,d_bin,d_start,d_end,y_bin,y_start,y_end);
    TH2D* h_xz = new TH2D("h_xz","X-Z;Width [pixel];Depth [mm]",x_bin,x_start,x_end,10*d_bin,d_start,d_end);
    TH2D* h_yz = new TH2D("h_yz","Z-Y;Depth [mm];Height [pixel]",10*d_bin,-1.0*d_end,-1.0*d_start,y_bin,y_start,y_end);
    TH2D* h_xy = new TH2D("h_xy","X-Y;Width [pixel];Height [pixel]",int(x_end-x_start),x_start,x_end,int(y_end-y_start),y_start,y_end);
    TH1D* h_x  = new TH1D("h_x","X;Width [pixel]",int(x_end-x_start),x_start,x_end);
    TH1D* h_y  = new TH1D("h_y","Y;Height [pixel]",int(y_end-y_start),y_start,y_end);
    TH1D* h_z  = new TH1D("h_z","Z;Depth [mm]",int(d_end-d_start),d_start,d_end);
    TH1D* h_px = new TH1D("h_px","X-axis;Time [s];Position [pixel]",1,-0.5,(double(f_end-f_start)/33.0)+0.5);
    TH1D* h_py = new TH1D("h_py","Y-axis;Time [s];Position [pixel]",1,-0.5,(double(f_end-f_start)/33.0)+0.5);
    TH1D* h_pz = new TH1D("h_pz","Z-axis;Time [s];Position [mm]",1,-0.5,(double(f_end-f_start)/33.0)+0.5);
    TGraph* g_px = new TGraph();
    TGraph* g_py = new TGraph();
    TGraph* g_pz = new TGraph();
    TLatex* tex_v = new TLatex();

    int c_div = 2;
    if(draw_mean_graph) c_div = 4;
    c_Meere->Divide(c_div,2);
    c_Meere->cd(1);
    h_3d->Draw();
    //gPad->SetTheta(45);
    //gPad->SetPhi(-30);
    c_Meere->cd(2);
    h_xz->Draw();
    c_Meere->cd(c_div+1);
    h_yz->Draw();
    h_xy->SetMaximum(d_max);
    h_xy->SetMinimum(d_min);
    c_Meere->cd(c_div+2);
    h_xy->Draw("colz");
    if(draw_mean_graph){
        c_Meere->cd(3);
        h_px->SetMaximum(x_max);
        h_px->SetMinimum(x_min);
        h_px->Draw();
        g_px->Draw("same");
        c_Meere->cd(4);
        h_py->SetMaximum(y_max);
        h_py->SetMinimum(y_min);
        h_py->Draw();
        g_py->Draw("same");
        c_Meere->cd(7);
        h_pz->SetMaximum(d_max);
        h_pz->SetMinimum(d_min);
        h_pz->Draw();
        g_pz->Draw("same");
	c_Meere->cd(8);
	tex_v->SetTextSize(0.13);
        tex_v->DrawLatex(0.1,0.7,"#font[12]{v_{x} =}        #scale[0.5]{pixel/s}");
        tex_v->DrawLatex(0.1,0.5,"#font[12]{v_{y} =}        #scale[0.5]{pixel/s}");
        tex_v->DrawLatex(0.1,0.3,"#font[12]{v_{z} =}        #scale[0.5]{mm/s}");
    }

    TLine x1_mean, y1_mean, x2_mean, y2_mean;
    double _x, _y, _d, _xm, _ym, _dm, _xe, _ye, _de, _xm_pre, _ym_pre, _dm_pre;
    _xm=160.0;
    _ym=120.0;
    _dm=3250.0;
    _xe=double(obj_x)*0.6;
    _ye=double(obj_y)*0.6;
    _de=double(obj_z)*0.6;
    if(_xe==0.0) _xe=320.0;
    if(_ye==0.0) _ye=240.0;
    if(_de==0.0) _de=6500.0;

    //cout << "double x_mean[" << f_end-f_start << "];" << endl;
    //cout << "double y_mean[" << f_end-f_start << "];" << endl;
    //cout << "double d_mean[" << f_end-f_start << "];" << endl;

    for(int ff=f_start;ff<f_end;++ff){

	h_3d->Reset();
	h_yz->Reset();
	h_xz->Reset();
	h_xy->Reset();
	h_x->Reset();
	h_y->Reset();
	h_z->Reset();

        //FIXME - Check size of 'long'
	//fread(&m_nFrameIndex, sizeof(long), 1, m_pLoadFile);
	fread(&m_nFrameIndex, sizeof(unsigned int), 1, m_pLoadFile);
	fread(&m_nTimestamp, sizeof(ULONGLONG), 1, m_pLoadFile);
	fread(m_pCameraBuf[0], 320*240*sizeof(unsigned short), 1, m_pLoadFile);
	fread(m_pCameraBuf[1], 320*240*sizeof(unsigned short), 1, m_pLoadFile);
	if(m_stFrameHeader.nDeviceType == 1)//0:Depth, 1:Depth+RGB
	{
            //FIXME - Check size of 'long'
	    //RGB
	    //fread(&m_nRGBFrameIndex, sizeof(long), 1, m_pLoadFile);
	    fread(&m_nRGBFrameIndex, sizeof(unsigned int), 1, m_pLoadFile);
	    fread(&m_nRGBTimestamp, sizeof(ULONGLONG), 1, m_pLoadFile);
	    fread(m_pRGBCameraBuf, sizeof(unsigned char), 640*480*3, m_pLoadFile);
	}

	if(obj_x>0 || obj_y>0 || obj_z>0){
	    for(int y=0;y<240;++y){
  	        for(int x=320;x>0;--x){
		    _x = 320.0-x;
		    _y = 240.0-y;
		    _d = m_pCameraBuf[1][y*320+x]/1.0;	
		    if((_d>d_min && _d<d_max) && (_x>x_min && _x<x_max) && (_y>y_min && _y<y_max)){
		        if(obj_x>0) h_x->Fill(_x);
		        if(obj_y>0) h_y->Fill(_y);
		 	if(obj_z>0) h_z->Fill(_d);
		    }
		}
	    }
   	    if(obj_x>0) _xm = h_x->GetMean(); 
   	    if(obj_y>0) _ym = h_y->GetMean(); 
	    if(obj_z>0) _dm = h_z->GetMean(); 
        }

	for(int y=0;y<240;++y){
	    for(int x=320;x>0;--x){
		_x = 320.0-x;
		_y = 240.0-y;
		_d = m_pCameraBuf[1][y*320+x]/1.0;	
		if((_d>_dm-_de && _d<_dm+_de) && (_x>_xm-_xe && _x<_xm+_xe) && (_y>_ym-_ye && _y<_ym+_ye)){
		    h_3d->Fill(_x,_d,_y);
		    h_yz->Fill(-1*_d,_y);
		    h_xz->Fill(_x,_d);
		    h_xy->Fill(_x,_y,_d);
		}
	    }
	}
        _xm = h_3d->GetMean(1);
	_dm = h_3d->GetMean(2); 
	_ym = h_3d->GetMean(3); 

	c_Meere->cd(1);
	gPad->Modified();
	c_Meere->cd(2);
	if(draw_mean_line){
            x1_mean = TLine(x_start,_dm,x_end,_dm);
    	    x1_mean.SetLineColor(kRed);
	    x1_mean.Draw("same");
            y1_mean = TLine(_xm,d_start,_xm,d_end);
	    y1_mean.SetLineColor(kRed);
	    y1_mean.Draw("same");
        }
	gPad->Modified();
	c_Meere->cd(c_div+1);
        if(draw_mean_line){
            x2_mean = TLine(-1.0*d_end,_ym,-1.0*d_start,_ym);
	    x2_mean.SetLineColor(kRed);
	    x2_mean.Draw("same");
            y2_mean = TLine(-1*_dm,y_start,-1*_dm,y_end);
	    y2_mean.SetLineColor(kRed);
	    y2_mean.Draw("same");
        }
	gPad->Modified();
	c_Meere->cd(c_div+2);
	gPad->Modified();

	if(draw_mean_graph){
	    g_px->SetPoint(ff,double(ff)/33.0,_xm);
	    g_py->SetPoint(ff,double(ff)/33.0,_ym);
	    g_pz->SetPoint(ff,double(ff)/33.0,_dm);
	    c_Meere->cd(3);
	    gPad->Modified();
	    c_Meere->cd(4);
	    gPad->Modified();
	    c_Meere->cd(7);
	    gPad->Modified();
	    c_Meere->cd(8);
	    if(ff==f_start){
		_xm_pre = _xm;
		_ym_pre = _ym;
		_dm_pre = _dm;
	    }
	    TLatex* tex_vx = new TLatex(0.6,0.7,Form("%0.2f",(_xm-_xm_pre)*33.0));
	    TLatex* tex_vy = new TLatex(0.6,0.5,Form("%0.2f",(_ym-_ym_pre)*33.0));
	    TLatex* tex_vz = new TLatex(0.6,0.3,Form("%0.2f",(_dm-_dm_pre)*33.0));
  	    tex_vx->SetTextSize(0.10);
	    tex_vy->SetTextSize(0.10);
	    tex_vz->SetTextSize(0.10);
	    tex_vx->SetTextAlign(31);
	    tex_vy->SetTextAlign(31);
	    tex_vz->SetTextAlign(31);
	    tex_vx->Draw("same");
	    tex_vy->Draw("same");
	    tex_vz->Draw("same");
	    _xm_pre = _xm;
	    _ym_pre = _ym;
	    _dm_pre = _dm;
	    gPad->Modified();
	    c_Meere->Update();
	    c_Meere->SaveAs(Form("./%s/%s_%03d.png",output_path.c_str(),input_file.c_str(),ff));
	    delete tex_vx;
	    delete tex_vy;
	    delete tex_vz;
	    gPad->Modified();
	    c_Meere->Update();
        } else {
	    c_Meere->Update();
	    c_Meere->SaveAs(Form("./%s/%s_%03d.png",output_path.c_str(),input_file.c_str(),ff));
	}
	//cout << "x_mean[" << ff-f_start << "] = " << _xm << ";" << endl;
	//cout << "y_mean[" << ff-f_start << "] = " << _ym << ";" << endl;
	//cout << "d_mean[" << ff-f_start << "] = " << _dm << ";" << endl;
    }

}
