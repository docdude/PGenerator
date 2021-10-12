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

#include "ofApp.h"

 

/*
 ##########################################################
 #                           Setup                        #
 ##########################################################
*/
void ofApp::setup(){
 ofSetBackgroundAuto(true); 
 ofBackground(def_r,def_g,def_b);
 ofHideCursor();
 /* Pid Creation */
 std::ofstream pidfile (pid_file);
 pidfile << getpid();
 pidfile.close();
}

/*
 ##########################################################
 #                           Update                       #
 ##########################################################
*/
void ofApp::update(){
 std::vector<std::string> dimensions;
 std::vector<std::string> rgb;
 std::vector<std::string> rgbb;
 std::vector<std::string> positions;
 std::string str; 

 if(open_file) {
  frame=frame_to_draw=entered=0;
  p_name=m_name="";
  n_draw[frame]=0;
  ofApp::log("\n\n");   
  ofApp::log("***********************************");
  ofApp::log("*             Open File           *");
  ofApp::log("***********************************");
  std::ifstream file(path);
  while (std::getline(file, str)) {
   entered=1;
   open_file=0;
   last_frame_time=0;
   std::vector<std::string> el;
   boost::split(el, str, boost::is_any_of("="));
   if(el[0] == "DRAW")
    draw_type=el[1];
   if(el[0] == "TEXT")
    text_to_write=el[1];
   if(el[0] == "PATTERN_NAME")
    p_name=el[1];
   if(el[0] == "MOVIE_NAME")
    m_name=el[1];
   if(el[0] == "IMAGE")
    img_file=el[1];
   if(el[0] == "ROTATE")
    img_rotate=boost::lexical_cast<int>(el[1]);
   if(el[0] == "DIM") {
    boost::split(dimensions, el[1], boost::is_any_of(","));
    dim1=boost::lexical_cast<int>(dimensions[0]);
    dim2=boost::lexical_cast<int>(dimensions[1]);
   }
   if(el[0] == "RESOLUTION")
    resolution=boost::lexical_cast<int>(el[1]);
   if(el[0] == "RGB") {
    boost::split(rgb, el[1], boost::is_any_of(","));
    red=boost::lexical_cast<int>(rgb[0]);
    green=boost::lexical_cast<int>(rgb[1]);
    blue=boost::lexical_cast<int>(rgb[2]);
   }
   if(el[0] == "POSITION") {
    boost::split(positions, el[1], boost::is_any_of(","));
    position_x=boost::lexical_cast<int>(positions[0]);
    position_y=boost::lexical_cast<int>(positions[1]);
   }
   if(el[0] == "BG") {
    boost::split(rgbb, el[1], boost::is_any_of(","));
    redb=boost::lexical_cast<int>(rgbb[0]);
    greenb=boost::lexical_cast<int>(rgbb[1]);
    blueb=boost::lexical_cast<int>(rgbb[2]);
   }
   if(el[0] == "FRAME_NAME") {
    name=el[1];
    arr_name[frame]=name;
   }
   if(el[0] == "FRAME") {
    arr_frame_time[frame]=boost::lexical_cast<int>(el[1])*1000;
    frame++;
    n_draw[frame]=0;
   }
   if(el[0] == "FRAME_DURATION")
    arr_frame_duration[frame]=boost::lexical_cast<int>(el[1])*1000;
   if(el[0] == "END") {
    ofApp::set_values();
    n_draw[frame]++;
    dim1=dim2=0;
   }
  }
 }
 image_save=tmp_dir+ofToString("running/")+ofToString(p_name)+".save";
 const char * save = image_save.c_str();
 ifstream s(save);
 if (s.good()) {
  save_images=1;
  s.close();
 }
}

/*
 ##########################################################
 #                           Draw                         #
 ##########################################################
*/
void ofApp::draw(){
 if(entered == 0)
  return;
 for(to_draw=0;to_draw<n_draw[i];to_draw++) {
  string return_file=tmp_dir+ofToString("/running/return");
  const char * return_file_char = return_file.c_str();
  ifstream r(return_file_char);
  if (r.good()) {
   r.close();
   i=0;
   open_file=1;
   save_images=0;
   unlink(return_file_char);
   return;
  }
  char buffer[255];
  sprintf(buffer,"Doing the frame %d and the Draw %d",i,to_draw);
  ofApp::log(buffer);
  sprintf(buffer,"RGB: %d %d %d",arr_red[i][to_draw],arr_green[i][to_draw],arr_blue[i][to_draw]);
  ofApp::log(buffer);
  sprintf(buffer,"Background: %d %d %d",arr_redbg[i][to_draw],arr_greenbg[i][to_draw],arr_bluebg[i][to_draw]);
  ofApp::log(buffer);
  sprintf(buffer,"DrawType: %d",arr_draw[i][to_draw]);
  ofApp::log(buffer);
  sprintf(buffer,"Dim: %d %d",arr_dim1[i][to_draw],arr_dim2[i][to_draw]);
  ofApp::log(buffer);
  sprintf(buffer,"Resolution: %d",arr_resolution[i][to_draw]);
  ofApp::log(buffer);
  sprintf(buffer,"Position: %d %d",arr_posx[i][to_draw],arr_posy[i][to_draw]);
  ofApp::log(buffer);
  ofSetColor(arr_red[i][to_draw],arr_green[i][to_draw],arr_blue[i][to_draw]);
  if(arr_redbg[i][to_draw] != -1)
   ofBackground(arr_redbg[i][to_draw],arr_greenbg[i][to_draw],arr_bluebg[i][to_draw]);
  //else
  //ofBackground(def_r,def_g,def_b);
  if(arr_draw[i][to_draw] == 0)
   ::exit(0);
  if(arr_draw[i][to_draw] ==  1)
   ofApp::rectangle();
  if(arr_draw[i][to_draw] ==  2)
   ofApp::circle();
  if(arr_draw[i][to_draw] ==  3)
   ofApp::triangle();
  if(arr_draw[i][to_draw] ==  4)
   ofApp::text();
  if(arr_draw[i][to_draw] ==  5)
   ofApp::image();
  if(last_frame_time == 0)
   last_frame_time=ofGetSystemTimeMicros();
 }
 if(save_images && i!=0 && !first_done)
  save_images=0;
 if (save_images) {
  first_done=1;
  //int n_frame=ofGetFrameNum();
  n_frame++;
  if(n_frame != 0 && n_frame > 3) {
   string movie_time=ofToString(arr_frame_time[i]);
   if(arr_frame_duration[i])
    movie_time=ofToString(arr_frame_duration[i]);
   movie_name=p_name;
   if(!m_name.empty())
    movie_name=m_name;
   //string image_name=tmp_dir+ofToString("/frames/Img_")+ofToString(movie_name)+",,,,"+ofToString(i)+"-"+arr_name[i]+",,,,"+movie_time+".png";
   string image_name_tmp=tmp_dir+ofToString("/tmp/")+ofToString(i+1)+"-"+arr_name[i]+",,,,"+movie_time+".png";
   string image_name=tmp_dir+ofToString("/frames/")+ofToString(i+1)+"-"+arr_name[i]+",,,,"+movie_time+".png";
   const char * c = image_name.c_str();
   ifstream p(c);
   if(!p.good()) {
    img.grabScreen(0,0,ofGetWindowWidth(),ofGetWindowHeight());
    img.save(image_name_tmp);
    rename(image_name_tmp.c_str(),image_name.c_str());
   } else
    p.close();
  }
 }
 if(arr_frame_time[i] != 0) {
  unsigned long long diff=ofGetSystemTimeMicros()-last_frame_time;
  if(diff > arr_frame_time[i]) {
   i++;
   last_frame_time=0;
   n_frame=0;
  }
 }


 if(i< frame)
  ofApp::log("\n");
 if(i == frame) {
  i=0;
  open_file=1;
  save_images=0;
  if(first_done) {
   unlink(image_save.c_str());
   //string image_done=tmp_dir+ofToString("/frames/Img_")+ofToString(movie_name)+",,,,.done";
   string image_done=tmp_dir+ofToString("/frames/done");
   const char * save_done = image_done.c_str();
   std::ofstream outfile (save_done);
   outfile << movie_name << std::endl;
   outfile.close();
   first_done=0;
  }
 }
}


/*
 ##########################################################
 #                        Set Values                      #
 ##########################################################
*/
void ofApp::set_values () {
 int draw_num=0;
 if(draw_type == "RECTANGLE") draw_num=1;
 if(draw_type == "CIRCLE")    draw_num=2;
 if(draw_type == "TRIANGLE")  draw_num=3;
 if(draw_type == "TEXT")      draw_num=4;
 if(draw_type == "IMAGE")     draw_num=5;
 arr_text[frame][n_draw[frame]]=text_to_write;
 arr_red[frame][n_draw[frame]]=red;
 arr_green[frame][n_draw[frame]]=green;
 arr_blue[frame][n_draw[frame]]=blue;
 arr_redbg[frame][n_draw[frame]]=redb;
 arr_greenbg[frame][n_draw[frame]]=greenb;
 arr_bluebg[frame][n_draw[frame]]=blueb;
 arr_draw[frame][n_draw[frame]]=draw_num;
 arr_dim1[frame][n_draw[frame]]=dim1;
 arr_dim2[frame][n_draw[frame]]=dim2;
 arr_posx[frame][n_draw[frame]]=position_x;
 arr_posy[frame][n_draw[frame]]=position_y;
 arr_resolution[frame][n_draw[frame]]=resolution;
 arr_image[frame][n_draw[frame]]=img_file;
 arr_rotate[frame][n_draw[frame]]=img_rotate;
}

/*
 ##########################################################
 #                       Rectangle                        #
 ##########################################################
*/
void ofApp::rectangle () {
 if(arr_posx[i][to_draw] == -1) {
  arr_posx[i][to_draw]=(ofGetWindowWidth()-arr_dim1[i][to_draw])/2;
  arr_posy[i][to_draw]=(ofGetWindowHeight()-arr_dim2[i][to_draw])/2;
 }
 ofDrawRectangle(arr_posx[i][to_draw],arr_posy[i][to_draw],arr_dim1[i][to_draw],arr_dim2[i][to_draw]);
}

/*
 ##########################################################
 #                       Circle                           #
 ##########################################################
*/
void ofApp::circle() {
 ofSetCircleResolution(arr_resolution[i][to_draw]);
 if(arr_posx[i][to_draw] == -1) {
  arr_posx[i][to_draw]=ofGetWindowWidth()/2;
  arr_posy[i][to_draw]=ofGetWindowHeight()/2;
 }
 ofDrawCircle(arr_posx[i][to_draw],arr_posy[i][to_draw],arr_dim1[i][to_draw]);
}

/*
 ##########################################################
 #                       Triangle                         #
 ##########################################################
*/
void ofApp::triangle() {
 if(arr_posx[i][to_draw] == -1) {
  arr_posx[i][to_draw]=ofGetWindowWidth()/2;
  arr_posy[i][to_draw]=ofGetWindowHeight()/2;
 }
 ofDrawTriangle(arr_posx[i][to_draw],arr_posy[i][to_draw]-arr_dim1[i][to_draw],arr_posx[i][to_draw]-arr_dim1[i][to_draw],arr_posy[i][to_draw]+arr_dim1[i][to_draw],arr_posx[i][to_draw]+arr_dim1[i][to_draw],arr_posy[i][to_draw]+arr_dim1[i][to_draw]);
}

/*
 ##########################################################
 #                         Text                           #
 ##########################################################
*/
void ofApp::text() {
 myfont.load(text_font,arr_dim1[i][to_draw]);
 int width=myfont.stringWidth(arr_text[i][to_draw]);
 int height=myfont.stringHeight(arr_text[i][to_draw]);
 if(arr_posx[i][to_draw] == -1) {
  arr_posx[i][to_draw]=(ofGetWindowWidth()-width)/2;
  arr_posy[i][to_draw]=ofGetWindowHeight()/2;
 }
 myfont.drawString(arr_text[i][to_draw],arr_posx[i][to_draw],arr_posy[i][to_draw]);
}

/*
 ##########################################################
 #                         Image                          #
 ##########################################################
*/

void ofApp::image() {

 int scale_dim1 = ofGetWindowWidth()/arr_dim1[i][to_draw];
 int scale_dim2 = ofGetWindowHeight()/arr_dim2[i][to_draw];
 
 if(arr_posx[i][to_draw] == -1) {
  arr_posx[i][to_draw]=(ofGetWindowWidth()-(arr_dim1[i][to_draw]*scale_dim1))/2;
  arr_posy[i][to_draw]=(ofGetWindowHeight()-(arr_dim2[i][to_draw]*scale_dim2))/2;

 }


if (ofxRPI4Window::isHDR) {
 float_img.clear();

	float_img.load(arr_image[i][to_draw]);
   			    int width = float_img.getPixels().getWidth();
    int height = float_img.getPixels().getHeight();
    int channels = float_img.getPixels().getNumChannels();
    
    ofLogNotice("image specs") << "width " << width << " height " << height << " channels " << channels;
	ofLogNotice("image specs2") << "x " << arr_posx[i][to_draw] << " y " << arr_posy[i][to_draw];
	float_img.rotate90(arr_rotate[i][to_draw]);
     ofSetColor(255,255,255,255);

    float_img.update();
    float_img.draw(arr_posx[i][to_draw],arr_posy[i][to_draw],arr_dim1[i][to_draw]*scale_dim1,arr_dim2[i][to_draw]*scale_dim2); 	  
} else {
	 img.clear();

	img.load(arr_image[i][to_draw]);
   			    int width = img.getPixels().getWidth();
    int height = img.getPixels().getHeight();
    int channels = img.getPixels().getNumChannels();
    
    ofLogNotice("image specs") << "width " << width << " height " << height << " channels " << channels;
	ofLogNotice("image specs2") << "x " << arr_posx[i][to_draw] << " y " << arr_posy[i][to_draw];
	img.rotate90(arr_rotate[i][to_draw]);
     ofSetColor(255,255,255,255);

    img.update();
    img.draw(arr_posx[i][to_draw],arr_posy[i][to_draw],arr_dim1[i][to_draw]*scale_dim1,arr_dim2[i][to_draw]*scale_dim2); 	  

}


}
void image_local() {
//	ofFloatImage float_image;
 int w;
  int h;
  int num_channels;
 //   auto window_type = std::make_shared<ofxRPI4Window>();	

 ofxTinyEXR exrIO;
 //float_image.clear();
//glEnable(GL_DEPTH_TEST);
ofFloatPixels pix;
  exrIO.loadHDRImage(pix, w, h, num_channels, "/var/lib/PGenerator/images/Ted Lightspace CMS Calibration Disk/Pre Calibration Tools/AtriumNight_oA9D.hdr"); 

 int x =(ofGetWindowWidth()- w)/2;
 int y =(ofGetWindowHeight()- h)/2;
#if 0
        
        ofFbo::Settings settings;
        settings.width                      = ofGetWidth();
        settings.height                     = ofGetHeight();
   //     settings.internalformat             = GL_RGB32F;
        settings.numSamples                 = 0;
        settings.useDepth                   = true;
        settings.useStencil                 = true;
        settings.depthStencilAsTexture      = true;
        settings.textureTarget              = GL_TEXTURE_2D;
        settings.depthStencilInternalFormat = GL_DEPTH_COMPONENT16;
  //      settings.minFilter                  = GL_NEAREST;
   //     settings.maxFilter                  = GL_NEAREST;
  //      settings.wrapModeHorizontal         = GL_CLAMP_TO_BORDER;
   //     settings.wrapModeVertical           = GL_CLAMP_TO_BORDER;
        
#endif

size_t n = w * h * num_channels;
std::cout << "The size is " << n << '\n';


ofTexture tex;
 tex.isAllocated();//
 tex.loadData(pix);
// for (size_t i = 0; i < n; i++) {cout << hex << setfill('0');  cout << setw(2) << reinterpret_cast<int*>(&pix.getData()[i]);}
//float_image.allocate(800,800,OF_IMAGE_COLOR);
//ofFbo floatFbo;
//floatFbo.allocate(800, 800, GL_RGB, 0);
//floatFbo.allocate(settings);
   //     floatFbo.createAndAttachTexture(GL_RGB, 1);

     // floatFbo.createAndAttachRenderbuffer(GL_RGB32F, 3);
      //  floatFbo.checkStatus();
  //    float_image.rotate90(arr_rotate[i][to_draw]);
 // ofFloatPixels floatpix;
//  floatpix.allocate( 800, 800, OF_IMAGE_COLOR );   
//  floatFbo.readToPixels(floatpix);

 //     ofSetColor(255,255,255);
//float_image.getPixels().setFromPixels(floatpix.getData(), 800, 800, OF_IMAGE_COLOR);
 //     printf("frameBufferData:\n");
  //  for (int i = 0; i < 200; ++i)
  //  {
  //      printf("%i ", floatpix[i]);
  //  }
  //  printf("\n");
    //  tex.update();
	   ofSetColor(255,255,255);
      tex.draw(x,y,w,h); 	  
	  
	  
}

/*
 ##########################################################
 #                          Log                           #
 ##########################################################
*/
void ofApp::log(std::string str) {
  return;
  ifstream f(DOLOG);
  if (f.good()) {
   f.close();
  } else {
   f.close();
   return;
  }   
  std::cout << str << std::endl;
}
