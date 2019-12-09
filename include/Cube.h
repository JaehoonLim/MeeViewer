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
bool zoom_all = true; // X & Y & D
bool zoom_x = false;
bool zoom_y = false;
bool zoom_d = false;

// Object Size Cut
int obj_x = 150; // NoCut:0 / mm
int obj_y = 150; // NoCut:0 / mm
int obj_z = 50; // NoCut:0 / mm

// Color Picture
bool draw_picture = false;

// Color Mask
bool do_mask = false;
bool mask_only = false; // Draw mask only // false : draw mask with red color
int mask_r  = 20; // red
int mask_g  = 20; // green
int mask_b  = 20; // blue
int mask_re = 20; // red +/- range
int mask_ge = 20; // green +/- range
int mask_be = 20; // blue +/- range
int mask_blur = 10; // pixel

// Draw Object Position Mean Line
bool draw_mean_line = true;

// Draw Object Position Graph
bool draw_mean_graph = true;


