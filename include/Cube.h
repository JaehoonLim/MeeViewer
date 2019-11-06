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
