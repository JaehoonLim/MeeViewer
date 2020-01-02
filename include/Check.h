// Input/Output File
string input_path = "Input";
string input_file = "black.mee";
string output_path = "Output";

// Video Trim
int f_start = 60;
int f_end = 0; // NoCut:0 (33 FPS)

// Video Crop
int x_min = 140;
int x_max = 200; // NoCut:0 / MAX:320 pixel
int y_min = 90;
int y_max = 140; // NoCut:0 / MAX:240 pixel
int d_min = 800;
int d_max = 1200; // NoCut:0 / MAX:6500 mm

// Video Zoom (Crop Area)
bool zoom_all = true; // X & Y & D
bool zoom_x = false;
bool zoom_y = false;
bool zoom_d = false;

// Object Size Cut
int obj_x = 0; // NoCut:0 / mm
int obj_y = 0; // NoCut:0 / mm
int obj_z = 0; // NoCut:0 / mm

// Color Picture
bool draw_picture = false;

// Color Mask
bool do_mask = false;
bool mask_only = false; // Draw mask only // false : draw mask with red color
int mask_r  = 255; // red
int mask_g  = 255; // green
int mask_b  = 0; // blue
int mask_re = 40; // red +/- range
int mask_ge = 40; // green +/- range
int mask_be = 40; // blue +/- range
int mask_blur = 5; // pixel

// Draw Object Position Mean Line
bool draw_mean_line = false;

// Draw Object Position Graph
bool draw_mean_graph = false;


