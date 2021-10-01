/*
 * Copyright (c) 2017-2018 Biasiotto Riccardo
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See the File README and COPYING for more detail about License
 *
*/

#pragma once
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "ofMain.h"
#include "ofxRPI4Window.h"
#include "ofxTinyEXR.h"

class ofApp : public ofBaseApp{
 public:
  char path[100]="/var/lib/PGenerator/operations.txt";
  char DOLOG[100]="/var/log/PGenerator/DOLOG";
  char save_frame_file[100]="/var/lib/PGenerator/save";
  char pid_file[100]="/var/run/PGenerator/PGeneratord.pid";
  char tmp_dir[100]="/var/lib/PGenerator/";
  char text_font[100]="/var/lib/PGenerator/fonts/PGenerator.ttf";
  ofTrueTypeFont myfont;
  ofFloatImage float_img;
  ofImage img;
  string image_save;
  string movie_name;
  int sleep_time;
  int first_done=0;
  std::string draw_type;
  std::string p_name;
  std::string name;
  std::string m_name;
  std::string text_to_write;
  std::string arr_text[2048][2048];
  std::string img_file;
  int save_images=0;
  int position_x;
  int img_rotate=0;
  int entered;
  int open_file=1;
  int i;
  int def_r=0;
  int def_g=0;
  int def_b=0;
  int n_frame;
  unsigned long long last_frame_time;
  int to_draw;
  int position_y;
  int arr_resolution[2048][2048];
  std::string arr_image[2048][2048];
  int arr_rotate[2048][2048];
  int arr_draw[2048][2048];
  std::string arr_name[2048];
  int arr_red[2048][2048];
  int arr_green[2048][2048];
  int arr_blue[2048][2048];
  int arr_redbg[2048][2048];
  int arr_greenbg[2048][2048];
  int arr_bluebg[2048][2048];
  int arr_dim1[2048][2048];
  int arr_dim2[2048][2048];
  int arr_posx[2048][2048];
  int arr_posy[2048][2048];
  unsigned long long arr_frame_time[2048];
  unsigned long long arr_frame_duration[2048];
  int dim1;
  int dim2;
  int width;
  int height;
  int resolution;
  int red;
  int green;
  int blue;
  int redb;
  int greenb;
  int blueb;
  int duration;
  int frame;
  int n_draw[2048];
  int frame_to_draw;
  ofVideoPlayer myPlayer;
  void setup();
  void update();
  void draw();
  void rectangle();
  void circle();
  void triangle();
  void text();
  void image();
  void set_values();

  void log(std::string);
 //
};
 void image_local();