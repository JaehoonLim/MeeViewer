#include "include/SimpleViewer.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"
//#include "include/Cube.h" // Cube Test
//#include "include/Wave.h" // Wave Test
#include "include/Check.h" // Cube Test

// Main Function Start
void MeeViewer(){

    gROOT->SetStyle("Plain");
    gStyle->SetOptStat(0);
    gStyle->SetMarkerSize(0.8);
    gStyle->SetPalette(1);

    m_pLoadFile = fopen((input_path+"/"+input_file).c_str(),"rb");
    input_file.erase(input_file.end()-4,input_file.end());
    fread(&m_stFrameHeader, sizeof(m_stFrameHeader), 1, m_pLoadFile);
    bool has_RGB = false;
    if(m_stFrameHeader.nDeviceType==1){
	cout << "Depth+RGB" << endl;
	has_RGB = true;
    } else {
	cout << "Depth only" << endl;
    }
    fseek(m_pLoadFile, sizeof(m_stFrameHeader), SEEK_SET);

    for(int i=0; i<2; ++i){//0:AMPLITUDE, 1:Depth
	m_pCameraBuf[i]	= (unsigned short *)malloc(320*240*sizeof(unsigned short));
	memset(m_pCameraBuf[i], 0, 320*240*sizeof(unsigned short));

    }
    m_pRGBCameraBuf    = (unsigned char *)malloc(640*480*3*sizeof(unsigned char));
    m_pRGBCameraBuf_re = (unsigned char *)malloc(640*480*3*sizeof(unsigned char));
    memset(m_pRGBCameraBuf, 0, 640*480*3*sizeof(unsigned char));
    memset(m_pRGBCameraBuf_re, 0, 640*480*3*sizeof(unsigned char));

    double x_start = 0.0;
    double x_end = 320;
    double y_start = 0.0;
    double y_end = 240;
    double d_start = 0.0;
    double d_end = 5000.0;

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
    int d_bin = int((d_end-d_start)/10);

    double fov_h = 84; // degree
    double fov_v = 64; // degree

    double depth_x = 160.0/tan(fov_h/2/180*TMath::Pi());
    double depth_y = 120.0/tan(fov_v/2/180*TMath::Pi());

    double depth_xy = 0.0;

    double xm_start = floor(d_max*(x_start-160)/depth_x/sqrt(2)/100.0)*100.0;
    double xm_end = ceil(d_max*(x_end-160)/depth_x/sqrt(2)/100.0)*100.0;
cout << "d_max : " << d_max << ", depth_x : " << depth_x << ", x_end-160 : " << x_end-160 << ", xm_end : " <<d_max*(x_end-160)/depth_x << endl;
    double ym_start = floor(d_max*(y_start-120)/depth_y/sqrt(2)/100.0)*100.0;
    double ym_end = ceil(d_max*(y_end-120)/depth_y/sqrt(2)/100.0)*100.0;

    int xm_bin = int((xm_end-xm_start)/10);
    int ym_bin = int((ym_end-ym_start)/10);

    // Output Picture Size
    int output_pic_width = 1000; // pixel
    int output_pic_height = 750; // pixel
    if(has_RGB && draw_picture){output_pic_width = 1500;}
    if(draw_mean_graph){output_pic_width = 2000;}
    bool check_save = false;

    TCanvas* c_Meere = new TCanvas("c_Meere","CubeEye Meere File Viewer",output_pic_width,output_pic_height);
    TH3D* h_3d =  new TH3D("h_3d","3D;Width [mm];Depth [mm];Height [mm]",xm_bin,xm_start,xm_end,d_bin,d_start,d_end,ym_bin,ym_start,ym_end);
    TH2D* h_xz = new TH2D("h_xz","X-Z;Width [mm];Depth [mm]",xm_bin,xm_start,xm_end,d_bin,d_start,d_end);
    TH2D* h_yz = new TH2D("h_yz","Z-Y;Depth [mm];Height [mm]",d_bin,-1.0*d_end,-1.0*d_start,ym_bin,ym_start,ym_end);
    TH2D* h_xy = new TH2D("h_xy","X-Y;Width [pixel];Height [pixel]",int(x_end-x_start),x_start,x_end,int(y_end-y_start),y_start,y_end);
    TH1D* h_x  = new TH1D("h_x","X;Width [mm]",int(xm_end-xm_start),xm_start,xm_end);
    TH1D* h_y  = new TH1D("h_y","Y;Height [mm]",int(ym_end-ym_start),ym_start,ym_end);
    TH1D* h_z  = new TH1D("h_z","Z;Depth [mm]",int(d_end-d_start),d_start,d_end);
    TH1D* h_px = new TH1D("h_px","X-axis;Time [s];Position [mm]",1,-0.5,(double(f_end-f_start)/33.0)+0.5);
    TH1D* h_py = new TH1D("h_py","Y-axis;Time [s];Position [mm]",1,-0.5,(double(f_end-f_start)/33.0)+0.5);
    TH1D* h_pz = new TH1D("h_pz","Z-axis;Time [s];Position [mm]",1,-0.5,(double(f_end-f_start)/33.0)+0.5);
    TH2D* h_xy_blank =  new TH2D("h_xy_blank","X-Y;Width [pixel];Height [pixel]",64,0,640,48,0,480); 
    TGraph* g_px = new TGraph();
    TGraph* g_py = new TGraph();
    TGraph* g_pz = new TGraph();
    TLatex* tex_v = new TLatex();

    int c_div = 2;
    if(has_RGB && draw_picture){c_div = 3;}
    if(draw_mean_graph){c_div = 4;}
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
    if(has_RGB && draw_picture){
        c_Meere->cd(c_div+3);
        h_xy_blank->Draw();
    }
    if(draw_mean_graph){
        c_Meere->cd(3);
        h_px->SetMaximum(xm_end);
        h_px->SetMinimum(xm_start);
        h_px->Draw();
        c_Meere->cd(4);
        h_py->SetMaximum(ym_end);
        h_py->SetMinimum(ym_start);
        h_py->Draw();
	if(has_RGB && draw_picture){
            c_Meere->cd(8);
            h_pz->SetMaximum(d_max);
            h_pz->SetMinimum(d_min);
            h_pz->Draw();
	} else {
            c_Meere->cd(7);
            h_pz->SetMaximum(d_max);
            h_pz->SetMinimum(d_min);
            h_pz->Draw();
	    c_Meere->cd(8);
	    tex_v->SetTextSize(0.13);
            tex_v->DrawLatex(0.1,0.7,"#font[12]{v_{x} =}          #scale[0.5]{mm/s}");
            tex_v->DrawLatex(0.1,0.5,"#font[12]{v_{y} =}          #scale[0.5]{mm/s}");
            tex_v->DrawLatex(0.1,0.3,"#font[12]{v_{z} =}          #scale[0.5]{mm/s}");
	}
    }
    c_Meere->Update();

    TLine x1_mean, y1_mean, x2_mean, y2_mean;
    double _x, _y, _d, _xm, _ym, _dm, _xe, _ye, _de, _xm_pre, _ym_pre, _dm_pre;
    _xm=0.0;
    _ym=0.0;
    _dm=3250.0;
    _xe=double(obj_x)*0.6;
    _ye=double(obj_y)*0.6;
    _de=double(obj_z)*0.6;
    if(_xe==0.0) _xe=10000.0;
    if(_ye==0.0) _ye=10000.0;
    if(_de==0.0) _de=10000.0;

    //cout << "double x_mean[" << f_end-f_start << "];" << endl;
    //cout << "double y_mean[" << f_end-f_start << "];" << endl;
    //cout << "double d_mean[" << f_end-f_start << "];" << endl;

    TImage* img;
    bool mask_img[640*480] = {false,};
    bool mask_img_blur[640*480] = {false,};
    int  mask_color[6] = {mask_r,mask_g,mask_b,mask_re,mask_ge,mask_be};
    bool mask_check = true;

    for(int ff=0;ff<f_end;++ff){

	h_3d->Reset();
	h_yz->Reset();
	h_xz->Reset();
	h_xy->Reset();
	h_x->Reset();
	h_y->Reset();
	h_z->Reset();
	check_save = false;

        //FIXME - Check size of 'long'
	//fread(&m_nFrameIndex, sizeof(long), 1, m_pLoadFile);
	fread(&m_nFrameIndex, sizeof(unsigned int), 1, m_pLoadFile);
	fread(&m_nTimestamp, sizeof(ULONGLONG), 1, m_pLoadFile);
	fread(m_pCameraBuf[0], 320*240*sizeof(unsigned short), 1, m_pLoadFile);
	fread(m_pCameraBuf[1], 320*240*sizeof(unsigned short), 1, m_pLoadFile);
	if(has_RGB)//0:Depth, 1:Depth+RGB
	{
            //FIXME - Check size of 'long'
	    //RGB
	    //fread(&m_nRGBFrameIndex, sizeof(long), 1, m_pLoadFile);
	    fread(&m_nRGBFrameIndex, sizeof(unsigned int), 1, m_pLoadFile);
	    fread(&m_nRGBTimestamp, sizeof(ULONGLONG), 1, m_pLoadFile);
	    fread(m_pRGBCameraBuf, sizeof(unsigned char), 640*480*3, m_pLoadFile);
	    for(int i=0;i<480;i++){
	        for(int j=0;j<640;j++){
		    mask_check = true;
	            for(int k=0;k<3;k++){
			if(m_pRGBCameraBuf[(640*3*i)+3*j+k] < mask_color[2-k] - mask_color[5-k]
			 || m_pRGBCameraBuf[(640*3*i)+3*j+k] > mask_color[2-k] + mask_color[5-k]){
			    if(do_mask) mask_check = false;
			}
		    }
		    mask_img[(640*i)+j]=mask_check;
		    mask_img_blur[(640*i)+j]=mask_check;
		}
	    }
	    if(do_mask){
		for(int i=mask_blur;i<480-mask_blur;i++){
		    for(int j=mask_blur;j<640-mask_blur;j++){
			if(mask_img[640*i+j]){
			    for(int k=-mask_blur;k<=mask_blur;k++){
			        for(int l=-mask_blur;l<=mask_blur;l++){
				    if((mask_blur*mask_blur)>=(k*k)+(l*l))mask_img_blur[(640*(i+k))+(j+l)] = true;
			        }
			    }
			}
		    }
		}
	    } // blur
	    for(int i=0;i<480;i++){
	        for(int j=0;j<640;j++){
	            for(int k=0;k<3;k++){
			if(do_mask){
			    if(mask_img_blur[(640*i)+j]){
			        if(mask_only){
  		                    m_pRGBCameraBuf_re[(640*3*i)+3*(639-j)+(2-k)] = m_pRGBCameraBuf[(640*3*i)+3*j+k];
			        } else {
				    if(k==2){
				        m_pRGBCameraBuf_re[(640*3*i)+3*(639-j)+(2-k)] = 255;
				    } else {
				        m_pRGBCameraBuf_re[(640*3*i)+3*(639-j)+(2-k)] = 0; 
				    }
			        }
			    } else {
			        if(mask_only){
		                    m_pRGBCameraBuf_re[(640*3*i)+3*(639-j)+(2-k)] = 0;
			        } else {
		                    m_pRGBCameraBuf_re[(640*3*i)+3*(639-j)+(2-k)] = m_pRGBCameraBuf[(640*3*i)+3*j+k];
			        }
			    }
			} else {
  		            m_pRGBCameraBuf_re[(640*3*i)+3*(639-j)+(2-k)] = m_pRGBCameraBuf[(640*3*i)+3*j+k];
			}
		    }
		}
	    }
	}

	if(ff>=f_start){ /**/
	if(obj_x>0 || obj_y>0 || obj_z>0){
	    for(int y=0;y<240;++y){
  	        for(int x=320;x>0;--x){
		    _x = 320.0-x;
		    _y = 240.0-y;
		    _d = m_pCameraBuf[1][y*320+x]/1.0;	
		    depth_xy = _d/sqrt(1+pow(_x/depth_x,2)+pow(_y/depth_y,2));
		    if((_d>d_min && _d<d_max) && (_x>x_min && _x<x_max) && (_y>y_min && _y<y_max) && (mask_img_blur[(640*2*y)+(640-int(2.06452*_x))] && (_x<311 || !do_mask))){
		        if(obj_x>0) h_x->Fill(depth_xy*(_x-160)/depth_x);
		        if(obj_y>0) h_y->Fill(depth_xy*(_y-120)/depth_y);
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
		if((_d>d_min && _d<d_max) && (_x>x_min && _x<x_max) && (_y>y_min && _y<y_max) && (mask_img_blur[(640*2*y)+(640-int(2.06452*_x))] && (_x<311 || !do_mask))){
		    depth_xy = _d/sqrt(1+pow(_x/depth_x,2)+pow(_y/depth_y,2));
		    _x = depth_xy*(_x-160)/depth_x;
		    _y = depth_xy*(_y-120)/depth_y;
		    if((_d>_dm-_de && _d<_dm+_de) && (_x>_xm-_xe && _x<_xm+_xe) && (_y>_ym-_ye && _y<_ym+_ye)){
		        h_3d->Fill(_x,_d,_y);
		        h_yz->Fill(-1*_d,_y);
		        h_xz->Fill(_x,_d);
		        h_xy->Fill(320.0-x, 240.0-y,_d);
                    }
		}
	    }
	}
        _xm = h_3d->GetMean(1);
	_dm = h_3d->GetMean(2); 
	_ym = h_3d->GetMean(3); 

	c_Meere->cd(1);
	gPad->Modified();
	c_Meere->cd(2);
	gPad->Modified();
	c_Meere->cd(c_div+1);
	gPad->Modified();
	c_Meere->cd(c_div+2);
	gPad->Modified();

	if(draw_mean_line){
	    c_Meere->cd(2);
            x1_mean = TLine(xm_start,_dm,xm_end,_dm);
    	    x1_mean.SetLineColor(kRed);
	    x1_mean.Draw("same");
            y1_mean = TLine(_xm,d_start,_xm,d_end);
	    y1_mean.SetLineColor(kRed);
	    y1_mean.Draw("same");
	    gPad->Modified();
	    c_Meere->cd(c_div+1);
            x2_mean = TLine(-1.0*d_end,_ym,-1.0*d_start,_ym);
	    x2_mean.SetLineColor(kRed);
	    x2_mean.Draw("same");
            y2_mean = TLine(-1*_dm,ym_start,-1*_dm,ym_end);
	    y2_mean.SetLineColor(kRed);
	    y2_mean.Draw("same");
        }

	if(has_RGB && draw_picture){
	    c_Meere->cd(c_div+3);
	    gPad->Modified();
	    stbi_write_bmp(Form("./%s/%s_%03d.bmp",output_path.c_str(),input_file.c_str(),ff),640,480,3,m_pRGBCameraBuf_re);
	    img = TASImage::Open(Form("./%s/%s_%03d.bmp",output_path.c_str(),input_file.c_str(),ff));
	    img->Draw("same");
	    gPad->Modified();
	}

	if(draw_mean_graph){
	    g_px->SetPoint(ff-f_start,double(ff-f_start)/33.0,_xm);
	    g_py->SetPoint(ff-f_start,double(ff-f_start)/33.0,_ym);
	    g_pz->SetPoint(ff-f_start,double(ff-f_start)/33.0,_dm);
	    c_Meere->cd(3);
            if(ff==f_start) g_px->Draw("same");
	    gPad->Modified();
	    c_Meere->cd(4);
            if(ff==f_start) g_py->Draw("same");
	    gPad->Modified();
	    if(has_RGB && draw_picture){
	        c_Meere->cd(8);
                if(ff==f_start) g_pz->Draw("same");
	        gPad->Modified();
	    } else {
	        c_Meere->cd(7);
                if(ff==f_start) g_pz->Draw("same");
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
		check_save = true;
	        delete tex_vx;
	        delete tex_vy;
	        delete tex_vz;
	    }
	}
	if(!check_save){
	    c_Meere->Update();
	    c_Meere->SaveAs(Form("./%s/%s_%03d.png",output_path.c_str(),input_file.c_str(),ff));
	    check_save = true;
	}
	x1_mean.Delete();
	y1_mean.Delete();
	x2_mean.Delete();
	y2_mean.Delete();
	//cout << "x_mean[" << ff-f_start << "] = " << _xm << ";" << endl;
	//cout << "y_mean[" << ff-f_start << "] = " << _ym << ";" << endl;
	//cout << "d_mean[" << ff-f_start << "] = " << _dm << ";" << endl;
        } // if(ff>=f_start){
    } // for(int ff=0;ff<f_end;++ff){

}
