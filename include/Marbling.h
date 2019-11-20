// Input/Output File
string input_path = "Input";
string input_file = "Marbling.mee";
string output_path = "Output";

// Video Trim
int f_start = 0;
int f_end = 100; // NoCut:0 (33 FPS)

// Video Crop
int x_min = 75;
int x_max = 215; // NoCut:0 / MAX:320 pixel
int y_min = 35;
int y_max = 175; // NoCut:0 / MAX:240 pixel
int d_min = 900;
int d_max = 1500; // NoCut:0 / MAX:6500 mm

// Video Zoom (Crop Area)
bool zoom_all = true; // X & Y & D
bool zoom_x = false;
bool zoom_y = false;
bool zoom_d = false;

// Object Size Cut
int obj_x = 0; // NoCut:0 / pixel
int obj_y = 0; // NoCut:0 / pixel
int obj_z = 0; // NoCut:0 / mm

// Draw Object Position Mean Line
bool draw_mean_line = false;

// Draw Object Position Graph
bool draw_mean_graph = false;
