/*
 * Copyright (c) 2017-2021 Biasiotto Riccardo
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

/* Start Patch RPI P4 */
#include "rgb2ycbcr.h"
/* End Patch RPI P4 */

/*
 ##########################################################
 #                           Setup                        #
 ##########################################################
*/
void ofApp::setup(){
 ofSetBackgroundAuto(true); 
 /* Start Patch RPI P4 */
 setBackground(def_r,def_g,def_b);
 /* End Patch RPI P4 */
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
   if(el[0] == "DRAW") {
    draw_type=el[1];
    /* Start Patch RPI P4 */
    ofxRPI4Window::colorspace_on = 1;
		    
    if ((ofxRPI4Window::shader_init && ofxRPI4Window::avi_info.output_format != 0) || (ofxRPI4Window::shader_init && ofxRPI4Window::is_std_DoVi)) {
     ofxRPI4Window::rgb2ycbcr_shader();
     ofxRPI4Window::shader_init=0;

    }
    /* End Patch RPI P4 */
   }
   if(el[0] == "TEXT")
    text_to_write=el[1];
   if(el[0] == "PATTERN_NAME")
    p_name=el[1];
   if(el[0] == "MOVIE_NAME")
    m_name=el[1];
   if(el[0] == "IMAGE") {
    img_file=el[1];
    /* Start Patch RPI P4 */
    ofxRPI4Window::colorspace_on=0;
//	shader.load("dovi_enc");
    if ((ofxRPI4Window::shader_init && ofxRPI4Window::avi_info.output_format != 0) || (ofxRPI4Window::shader_init && ofxRPI4Window::is_std_DoVi)) {
     ofxRPI4Window::rgb2ycbcr_shader();
     ofxRPI4Window::shader_init=0;
    }
    /* End Patch RPI P4 */
	 
   }
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
   /* Start Patch RPI P4 */
   if(el[0] == "BITS") {
    bits=boost::lexical_cast<int>(el[1]);
    ofxRPI4Window::bit_depth = bits;
    ofxRPI4Window::avi_info.max_bpc = bits;
    ofxRPI4Window::colorspace_on=1;
   }
   /* End Patch RPI P4 */
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
  if(arr_draw[i][to_draw] == 4) {
   sprintf(buffer,"DrawImage: %s",arr_text[i][to_draw].c_str());
   ofApp::log(buffer);
  }
  if(arr_draw[i][to_draw] == 5) {
   sprintf(buffer,"DrawImage: %s",arr_image[i][to_draw].c_str());
   ofApp::log(buffer);
  }
  sprintf(buffer,"Dim: %d %d",arr_dim1[i][to_draw],arr_dim2[i][to_draw]);
  ofApp::log(buffer);
  sprintf(buffer,"Resolution: %d",arr_resolution[i][to_draw]);
  ofApp::log(buffer);
  sprintf(buffer,"Position: %d %d",arr_posx[i][to_draw],arr_posy[i][to_draw]);
  ofApp::log(buffer);

  sprintf(buffer,"Bits: %d",arr_bits[i][to_draw]);
  ofApp::log(buffer);

  if(arr_draw[i][to_draw] == 0) 
   ::exit(0);
 /* Start Patch RPI P4 */

  ofApp::fbo_allocate(); //allocate framebuffer for DoVi background and patch
  
  ofApp::setBackground(arr_redbg[i][to_draw], arr_greenbg[i][to_draw], arr_bluebg[i][to_draw]);

  ofApp::setColor(arr_red[i][to_draw],arr_green[i][to_draw],arr_blue[i][to_draw]); 

  if(arr_draw[i][to_draw] !=  5) ofApp::shader_begin(0); //set for draw = 0
 /* End Patch RPI P4 */
  if(arr_draw[i][to_draw] ==  1)
   ofApp::rectangle();
  if(arr_draw[i][to_draw] ==  2)
   ofApp::circle();
  if(arr_draw[i][to_draw] ==  3)
   ofApp::triangle();
  if(arr_draw[i][to_draw] ==  4)
   ofApp::text();
  /* Start Patch RPI P4 */
  if(arr_draw[i][to_draw] != 5) ofApp::shader_end(0); //set for draw = 0

  if (arr_draw[i][to_draw] !=  5 && ofxRPI4Window::is_std_DoVi) ofApp::dovi_metadata_inject(ofxRPI4Window::bit_depth); //inject dovi metadata and draw patch
  /* End Patch RPI P4 */
  if(arr_draw[i][to_draw] ==  5)
   ofApp::image();
  /* Start Patch RPI P4 */
  if (arr_draw[i][to_draw] ==  5 && ofxRPI4Window::is_std_DoVi) ofApp::dovi_metadata_inject(ofxRPI4Window::bit_depth); //inject dovi metadata and draw patch
   /* End Patch RPI P4 */
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
	ofApp::YCbCr2RGB();
//	dovi_rpu_inject();
//ofApp::dovi_dump();
//	ofApp::dovi_metadata_inject(ofxRPI4Window::bit_depth);
	if( img.isAllocated() ){
 
	  img.draw(arr_posx[i][to_draw],arr_posy[i][to_draw],arr_dim1[i][to_draw],arr_dim2[i][to_draw]);
	}
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
 /* Start Patch RPI P4 */
 arr_bits[frame][n_draw[frame]]=bits;
 ofxRPI4Window::bit_depth=bits;
 ofxRPI4Window::avi_info.max_bpc = bits;
 /* End Patch RPI P4 */
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
// ofApp::shader_begin(0);
 ofDrawRectangle(arr_posx[i][to_draw],arr_posy[i][to_draw],arr_dim1[i][to_draw],arr_dim2[i][to_draw]);
 //ofApp::shader_end(0);
// if (ofxRPI4Window::bit_depth == 10) { 
 //    float_img.grabScreen(0,0,ofGetWindowWidth(),ofGetWindowHeight());
//	ofApp::YCbCr2RGB();
//	dovi_rpu_inject();
//ofApp::dovi_dump();
//	ofApp::dovi_metadata_inject(ofxRPI4Window::bit_depth);
	//float_img.save("/tmp/dovi_ycbcr.png");
//	if( float_img.isAllocated() ){
//		float_img.draw(0,0,ofGetWindowWidth(),ofGetWindowHeight());
//	}
// } else {
#if 0
		shader.begin(); //set DoVi Colorspace IPTPQc2, profile 8.2

		const float cmat_c_17[3] = {-0.976455271, 0.30258885, -1.13760722};

		ofMatrix3x3 cmat_16 = ofMatrix3x3(0.212630069,	0.715188177,	0.072181753, -0.117204231,	-0.394220255,	0.511424486, 0.511451232,	-0.464564191,	-0.046887042);
		cmat_16.transpose();
		//ofMatrix3x3 lms2rgb = ofMatrix3x3(0.753741384,     0.0457911342,    -0.00121179176,  0.198592365, 0.941773534,     0.017623404,     0.0475341678 ,   0.0125268921, 0.983739734 );
		//lms2rgb.transpose();

		shader.setUniformMatrix3f("cmat_16_0", cmat_16,1); // SET A UNIFORM
		shader.setUniform3fv("cmat_c_17_0", (const GLfloat*)cmat_c_17); // SET A UNIFORM

		ofDrawRectangle(arr_posx[i][to_draw],arr_posy[i][to_draw],arr_dim1[i][to_draw],arr_dim2[i][to_draw]);


		shader.end();
#endif
   //  img.grabScreen(0,0,ofGetWindowWidth(),ofGetWindowHeight());

//	ofApp::dovi_metadata_inject(ofxRPI4Window::bit_depth);
	//img.save("/tmp/dovi_ycbcr.png");
	//  img.clear();
	//img.load("/tmp/dovi_ycbcr.png");
//	if( img.isAllocated() ){

//		img.draw(0,0,ofGetWindowWidth(),ofGetWindowHeight());
	//}
	
// } 
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
 //int height=myfont.stringHeight(arr_text[i][to_draw]);
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
 if(arr_posx[i][to_draw] == -1) {
  arr_posx[i][to_draw]=(ofGetWindowWidth()-arr_dim1[i][to_draw])/2;
  arr_posy[i][to_draw]=(ofGetWindowHeight()-arr_dim2[i][to_draw])/2;
 }
 /* Start Patch RPI P4 */
 if (ofxRPI4Window::avi_info.max_bpc == 10 && ofxRPI4Window::isHDR) {
  float_img.clear();
  float_img.load(arr_image[i][to_draw]);
  float_img.rotate90(arr_rotate[i][to_draw]);
  ofSet10bitColor(1023,1023,1023,1023);
  float_img.update();
  ofApp::shader_begin(1); //set for image = 1
  float_img.draw(arr_posx[i][to_draw],arr_posy[i][to_draw],arr_dim1[i][to_draw],arr_dim2[i][to_draw]);
  ofApp::shader_end(1); //set for image = 1
 /* End Patch RPI P4 */
 } else {
  img.clear();
  img.load(arr_image[i][to_draw]);
  img.rotate90(arr_rotate[i][to_draw]);
  ofSetColor(255,255,255,255);
  img.update();
  /* Start Patch RPI P4 */
 ofApp::shader_begin(1); //set for image = 1
  /* End Patch RPI P4 */
#if 0
	//	if (ofxRPI4Window::isDoVi) {
		img.getTexture().bind();
		shader.begin(); //set DoVi Colorspace IPTPQc2, profile 8.2
		/*
		const float pivots[7] = {0.129032254, 0.353861183, 0.604105592, 0.854349971, 0.890518069, 0.906158328, 0.913978517};
		const float coeffs_5[8][4] = {{-0.0488376617, 1.99335372, -2.41716385, 0}, {-0.0141925812, 1.61829138, -1.53397191, 0}, {0.157061458, 0.63640213, -0.11302495, 0}, {0.25272119, 0.246226311, 0.27281332, 0}, 
									  {0.951621532, -1.35507894, 1.18898678, 0}, {6.41251612, -13.6188488, 8.07336903, 0}, {13.467535, -29.1869125, 16.6612244, 0}, {28.2321472, -61.8516273, 34.7264938, 0}};
	
		const float coeffs_8[8][4] = {{-0.500733018, 0, 0, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};
		const float mmr_9[8*6][4] = {{1.08411026, 3.80807829, 0.0881733894, 0}, {-3.23097038, -0.409078479, -1.31310081, 2.71297002}, {-0.241833091, -3.57880807, -0.108109117, 0},
									 {3.13198471, 0.869203091, 1.96561158, -9.30871677}, {-0.177356839, 1.48970401, 0.0908923149, 0}, {-0.510447979, -0.687603354, -0.934977889, 12.3544884}};
	
		const float coeffs_12[8][4] = {{-1.23833287, 0, 0, 3}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

		const float mmr_13[8*6][4] = {{3.52909589, 0.383154511, 5.50820637, 0}, {-1.02094889, -6.36386824, 0.194121242, 0.64683497}, {-2.57899785, -0.626081586, -6.05729723, 0}, {2.29143763, 9.14653015, -0.0507702827, -4.17724133},
				      	              {0.705404401, 0.341412306, 2.98387456, 0}, {-1.71712542, -4.91501331, 0.1465137, 6.38665438}};
	//	const float cmat_16[3][3] = {{1.16870117,      1.16870117,      1.16870117},{      0,    -0.213989258,    2.12023926},{      1.79919434,      -0.534912109 ,0}};
	*/
		const float cmat_c_17[3] = {0.0,    0.5 ,    0.5 };
	//	ofMatrix3x3 cmat_16 = ofMatrix3x3(1.0,      1.0,      1.0,    0,    -0.183100063, 1.814184249,     1.53948193,-0.457697932,0);
		ofMatrix3x3 cmat_16 = ofMatrix3x3(0.400049716, 4.4551878, 0.8061257,0.399855375, -4.85153294, 0.357382387 ,0.200094894, 0.396345139, -1.16350806);
//cmat_16.transpose();
            
//ofMatrix3x3 lms2rgb = ofMatrix3x3(0.753741384,     0.0457911342,    -0.00121179176,  0.198592365, 0.941773534,     0.017623404,     0.0475341678 ,   0.0125268921, 0.983739734 );
//lms2rgb.transpose();
	//	shader.setUniform1fv("pivots_4_0", (const GLfloat*) pivots); // SET A UNIFORM
	//	shader.setUniform4fv("coeffs_5_0", (const GLfloat*)coeffs_5); // SET A UNIFORM
	//	shader.setUniform4fv("coeffs_8_0", (const GLfloat*)coeffs_8); // SET A UNIFORM
	//	shader.setUniform4fv("mmr_9_0", (const GLfloat*)mmr_9); // SET A UNIFORM
	//	shader.setUniform4fv("mmr_13_0", (const GLfloat*)mmr_13); // SET A UNIFORM
		shader.setUniformMatrix3f("cmat_16_0", cmat_16,1); // SET A UNIFORM
		shader.setUniform3fv("cmat_c_17_0", (const GLfloat*)cmat_c_17); // SET A UNIFORM
	//	shader.setUniformMatrix3f("lms2rgb_18_0", lms2rgb,1); // SET A UNIFORM
//	}
#endif  

//	ofApp::YCbCr2RGB();
//	dovi_rpu_inject();
//ofApp::dovi_dump();

 img.draw(arr_posx[i][to_draw],arr_posy[i][to_draw],arr_dim1[i][to_draw],arr_dim2[i][to_draw]);
//ofApp::dovi_metadata_inject(ofxRPI4Window::bit_depth);
	//img.save("/tmp/test.png");
   // rename(image_name_tmp.c_str(),image_name.c_str());
 // img.draw(arr_posx[i][to_draw],arr_posy[i][to_draw],arr_dim1[i][to_draw],arr_dim2[i][to_draw]);
 
  /* Start Patch RPI P4 */
  ofApp::shader_end(1); //set for image = 1
  /* End Patch RPI P4 */
#if 0
 // 	if (ofxRPI4Window::isDoVi)   {
		shader.end();
		img.getTexture().unbind();
	//}
#endif
 }
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
  std::cout << "[" << boost::posix_time::microsec_clock::local_time().time_of_day().total_milliseconds() << "]: " << str << std::endl;
}

/*
 ##########################################################
 #                   Set Background                       #
 ##########################################################
*/
void ofApp::setBackground(int redbg, int greenbg, int bluebg) {
 if (ofxRPI4Window::isHDR && !ofxRPI4Window::isDoVi && !ofxRPI4Window::is_std_DoVi) { 
  if (ofxRPI4Window::bit_depth == 10) {  
   if(arr_redbg[i][to_draw] != -1) {
    if (ofxRPI4Window::avi_info.output_format != 0) {
     RGB data = RGB(redbg,greenbg,bluebg);
     YCbCr bg = RGB2YCbCr(data,10, ofxRPI4Window::avi_info.colorimetry, ofxRPI4Window::avi_info.rgb_quant_range);
     if (ofxRPI4Window::avi_info.output_format == 1) of10bitBackground(bg.Cb,bg.Cr,bg.Y);  //in YCbCr444, luminance is last channel
     if (ofxRPI4Window::avi_info.output_format == 2) of10bitBackground(bg.Y,bg.Cb,bg.Cr);  //in YCbCr422
    } else                                           of10bitBackground(redbg,greenbg,bluebg);
   }
  } else {
   if(arr_redbg[i][to_draw] != -1) {
    if (ofxRPI4Window::avi_info.output_format != 0) {
     RGB data = RGB(redbg,greenbg,bluebg);
     YCbCr bg = RGB2YCbCr(data,8,ofxRPI4Window::avi_info.colorimetry, ofxRPI4Window::avi_info.rgb_quant_range);
     if (ofxRPI4Window::avi_info.output_format == 1) ofBackground(bg.Cb,bg.Cr,bg.Y);  //in YCbCr444, luminance is last channel
     if (ofxRPI4Window::avi_info.output_format == 2) ofBackground(bg.Y,bg.Cb,bg.Cr);  //in YCbCr422
    } else                                           ofBackground(redbg,greenbg,bluebg);
   }
  }
 } else {
  if (ofxRPI4Window::bit_depth == 10) {  
   if(arr_redbg[i][to_draw] != -1) {
    if (ofxRPI4Window::avi_info.output_format != 0 || ofxRPI4Window::is_std_DoVi) {
     RGB data = RGB(redbg,greenbg,bluebg);
     YCbCr bg = RGB2YCbCr(data,10, ofxRPI4Window::avi_info.colorimetry, ofxRPI4Window::avi_info.rgb_quant_range);
     if (ofxRPI4Window::avi_info.output_format == 1) of10bitBackground(bg.Cb,bg.Cr,bg.Y);  //in YCbCr444, luminance is last channel
     if (ofxRPI4Window::avi_info.output_format == 2) of10bitBackground(bg.Y,bg.Cb,bg.Cr);  //in YCbCr422
	 if (ofxRPI4Window::is_std_DoVi == 1) 			 ofApp::setDoViBackground(redbg,greenbg,bluebg);	
    } else                                           of10bitBackground(redbg,greenbg,bluebg);
   }
  } else {
   if(arr_redbg[i][to_draw] != -1) {
    if (ofxRPI4Window::avi_info.output_format != 0 || ofxRPI4Window::is_std_DoVi) {
     RGB data = RGB(redbg,greenbg,bluebg);
     YCbCr bg = RGB2YCbCr(data,8,ofxRPI4Window::avi_info.colorimetry, ofxRPI4Window::avi_info.rgb_quant_range);
     if (ofxRPI4Window::avi_info.output_format == 1) ofBackground(bg.Cb,bg.Cr,bg.Y);  //in YCbCr444, luminance is last channel
     if (ofxRPI4Window::avi_info.output_format == 2) ofBackground(bg.Y,bg.Cb,bg.Cr);  //in YCbCr422
	 if (ofxRPI4Window::is_std_DoVi == 1) 			 ofApp::setDoViBackground(redbg,greenbg,bluebg);
    } else                                           ofBackground(redbg,greenbg,bluebg);
   }
  }
 }
}

/*
 ##########################################################
 #                     Set Color                          #
 ##########################################################
*/
void ofApp::setColor(int red, int green, int blue) {
 if (ofxRPI4Window::isHDR && !ofxRPI4Window::isDoVi && !ofxRPI4Window::is_std_DoVi) { 
  if (ofxRPI4Window::bit_depth == 10) ofSet10bitColor(red,green,blue);
  else                                ofSetColor(red,green,blue);
 } else {
  if (ofxRPI4Window::bit_depth == 10) ofSet10bitColor(red,green,blue);
  else                                ofSetColor(red,green,blue);
 }
}

/*
 ##########################################################
 #                       Shader Begin                     #
 ##########################################################
*/
void ofApp::shader_begin(int is_image) {
 if ((!ofxRPI4Window::shader_init && ofxRPI4Window::avi_info.output_format != 0) || (!ofxRPI4Window::shader_init && ofxRPI4Window::is_std_DoVi)) {
  if (is_image) {
	if (ofxRPI4Window::avi_info.max_bpc == 10 && ofxRPI4Window::isHDR) float_img.getTexture().bind();
	else 															         img.getTexture().bind();
  }
  if (ofxRPI4Window::is_std_DoVi && ofxRPI4Window::bit_depth == 10) fbo10.begin();
  if (ofxRPI4Window::is_std_DoVi && ofxRPI4Window::bit_depth == 8) fbo8.begin();  
  ofxRPI4Window::shader.begin();
  ofxRPI4Window::shader.setUniform1i("bits", ofxRPI4Window::bit_depth);
  ofxRPI4Window::shader.setUniform1i("colorimetry", ofxRPI4Window::avi_info.colorimetry);
  ofxRPI4Window::shader.setUniform1i("color_format", ofxRPI4Window::avi_info.output_format);
  ofxRPI4Window::shader.setUniform1i("rgb_quant_range", ofxRPI4Window::avi_info.rgb_quant_range);
  ofxRPI4Window::shader.setUniform1i("is_image", is_image);
  ofxRPI4Window::shader.setUniform1i("is_std_DoVi", ofxRPI4Window::is_std_DoVi);

 }		
}

/*
 ##########################################################
 #                        Shader End                      #
 ##########################################################
*/
void ofApp::shader_end(int is_image) {
 if ((!ofxRPI4Window::shader_init && ofxRPI4Window::avi_info.output_format != 0) || (!ofxRPI4Window::shader_init &&ofxRPI4Window::is_std_DoVi)) {
  ofxRPI4Window::shader.end();

  if (is_image) {
	if (ofxRPI4Window::avi_info.max_bpc == 10 && ofxRPI4Window::isHDR) float_img.getTexture().unbind();
    else 															         img.getTexture().unbind();
  }
  if (ofxRPI4Window::is_std_DoVi && ofxRPI4Window::bit_depth == 10) fbo10.end();
  if (ofxRPI4Window::is_std_DoVi && ofxRPI4Window::bit_depth == 8) fbo8.end();
 }		
}

 
void ofApp::YCbCr2RGB(){
	if (ofxRPI4Window::avi_info.output_format != 0) {
		int Y, Cb, Cr;
		//Getting pointer to pixel array of image
		unsigned char *pixels = img.getPixels().getData();
		//Calculate number of pixel components
		int width = img.getPixels().getWidth();
		int height = img.getPixels().getHeight();
		int channels = img.getPixels().getNumChannels();
		//Modify pixel array
		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
        
				//Read pixel (x,y) color components
				int index = channels * (x + width * y);

				if ( ofxRPI4Window::avi_info.output_format == 1) { //YCbCr444
					Cb = pixels[ index ];
					Cr = pixels[ index + 1 ];
					Y = pixels[ index + 2 ];
				}	
				if ( ofxRPI4Window::avi_info.output_format == 2) { 	//YCbCr422		
					Y = pixels[ index ];
					Cb = pixels[ index + 1 ];
					Cr = pixels[ index + 2 ];
				}

				YCbCr data = YCbCr(Y,Cb,Cr);
				RGB rgb = YCbCrToRGB(data,bits,ofxRPI4Window::avi_info.colorimetry, ofxRPI4Window::avi_info.rgb_quant_range);
				//Set red 
				pixels[ index ] = rgb.R;
				//Set green 
				pixels[ index + 1 ] = rgb.G;
				//Set blue 
				pixels[ index + 2 ] = rgb.B;
			}
		}
		//Calling img.update() to apply changes
		img.update();
	}
}
#if 0
void ofApp::dovi_rpu_inject() {
	if (ofxRPI4Window::avi_info.rgb_quant_range == 1 && ofxRPI4Window::avi_info.output_format == 2) {
		int Y=0, Cb=0, Cr=0;
		//Getting pointer to pixel array of image
		unsigned char *pixels = img.getPixels().getData();
		//Calculate number of pixel components
		int width = img.getPixels().getWidth();
		int height = img.getPixels().getHeight();
		int channels = img.getPixels().getNumChannels();
		int num_bits=0;
		unsigned char byte[] = {0x00,0x00,0x00,0x01,0x19,0x08,0x09,0x00,0x40,0x61,0xb6,0x50,0x6f,0x00,0x3f,0xf8,0x01,0xff,
								0xc0,0x0f,0xff,0xd0,0x00,0x00,0x10,0x00,0x00,0x1b,0x00,0x00,0x03,0x02,0x00,0x00,0x03,0x03,
								0x60,0x00,0x00,0x40,0x00,0x00,0x68,0x80,0x00,0x0c,0x7c,0x1a,0x44,0x80,0x03,0xf1,0x6c,0x11,
								0x0c,0x80,0x00,0x04,0x2f,0xa9,0x5c,0x00,0x00,0x03,0x00,0x00,0x20,0x00,0x00,0x03,0x00,0x20,
								0x00,0x00,0x03,0x01,0x0a,0xe7,0xfa,0x8f,0xfa,0x8f,0xfa,0x8d,0x0a,0xe7,0xfa,0x8f,0xfa,0x8f,
								0xfa,0x8d,0x0a,0xe7,0xff,0xfc,0x00,0x00,0x03,0x00,0x00,0x03,0x00,0x00,0x03,0x00,0x01,0x90,
								0x81,0xf7,0x38,0x05,0x45,0x30,0x08,0x01,0x3c,0x80,0x8c,0x00,0xc0,0x28,0x21,0x22,0x98,0x00,
								0x80,0x08,0x00,0x80,0x01,0x00,0x02,0x02,0x00,0x00,0x03,0x00,0x09,0x06,0x0f,0xa0,0x00,0x32,
								0x00,0x00,0x03,0x00,0x00,0x46,0xf1,0x99,0x5c,0x80 };

		unsigned char mask = 1; // Bit mask
		unsigned char bits[8];
		unsigned char total_bits[1500] = {0};
		int i, j = CHAR_BIT-1;
		int n = sizeof(byte)/sizeof(byte[0]);

		// Extract the bits
		for (int k=0; k < n; k++) {
			printf("byte 0x%02x : ",byte[k]);
			for ( i = 0; i < 8; i++,j--,mask = 1) {
			// Mask each bit in the byte and store it
				bits[i] =( byte[k] & (mask<<=j))  !=0;

				printf("%d", bits[i]);
				total_bits[num_bits] = bits[i];
				num_bits++;
			}

			puts("");
			j = CHAR_BIT-1;

		}
		printf("Total number of bits %d\n",num_bits);
		int z=0;
		for (int y=0; y<height; y++) {
			for (int x=0; x<width; x++) {
        
				//Read pixel (x,y) color components
				int index = channels * (x + width * y);
				
					Y = pixels[ index ];
					Cb = pixels[ index + 1 ];
					Cr = pixels[ index + 2 ];

					if (index == 0) { //set metadata identifier to 0b00, single packet contains all DM metadata
					//	printf("z=%d Before: Y %d ,Cb %d , Cr %d ",z, Y, Cb, Cr);

						Cb &= 0x3fe;
						Cr &= 0x3fe;
						printf("..");
					}
					if ((z <= num_bits) && (index >= 0)) {
					//	printf("z=%d Before: Y %d ,Cb %d , Cr %d ",z, Y, Cb, Cr);

						if (z % 2 == 0) {
							//printf(" even ");
							if (total_bits[z] == 0) {
								Cb &= 0x3fe;
							//	Cb = 
								printf(".");
							} else {
								Cb |= 1;
								printf("^");
							}
						}else{
							//printf(" odd ");
							if (total_bits[z] == 0) {
								Cr &= 0x3fe;
								printf(".");
							} else {
								Cr |= 1;
								printf("^");
							}
						}
		//		printf("==> After: Y %d ,Cb %d , Cr %d \n",Y, Cb, Cr);
						z++;
						
					pixels[ index + 1 ] = Cb;
					pixels[ index + 2 ] = Cr;
					}

			}
		}
puts("");
	}

	//Calling img.update() to apply changes
	img.update();

}


#endif

/*
 ##########################################################
 #                   Inject DoVi Metadata                 #
 ##########################################################
*/
void ofApp::dovi_metadata_inject(int bit_depth) {
		int Y=0, Cb=0, Cr=0;
		int width=0, height=0, channels=0;
		int num_bits=0;
		unsigned short *short_pixels;
		unsigned char *pixels;
		ofShortPixels short_pix;
		ofPixels pix;
		ofShortImage short_img;
		ofImage img;
		//Getting pointer to pixel array of image
		if (bit_depth == 10) {
	//		short_pix.allocate(ofGetWindowWidth(), ofGetWindowHeight(), OF_IMAGE_COLOR_ALPHA);
		//	float_img.clear(); 

		//	float_img.grabScreen(0,0,ofGetWindowWidth(),ofGetWindowHeight());

fbo10.readToPixels(short_pix);
			short_pixels = short_pix.getData();//float_img.getPixels().getData();
			//Calculate number of pixel components
			width = short_pix.getWidth();//float_img.getPixels().getWidth();
			height = short_pix.getHeight();//float_img.getPixels().getHeight();
			channels = short_pix.getNumChannels();//float_img.getPixels().getNumChannels();
		} else {
		//	img.clear(); 
		//	img.grabScreen(0,0,ofGetWindowWidth(),ofGetWindowHeight());
	//		pix.allocate(ofGetWindowWidth(), ofGetWindowHeight(), OF_IMAGE_COLOR_ALPHA);

fbo8.readToPixels(pix); 
			pixels = pix.getData();//img.getPixels().getData();
			//Calculate number of pixel components
			width = pix.getWidth();//img.getPixels().getWidth();
			height = pix.getHeight();//img.getPixels().getHeight();
			channels = pix.getNumChannels();//img.getPixels().getNumChannels();
		}



		/* 1024 bit sequence to set LG OLED to Calibrate DoVi, repeats 3 times, ends in CRC32-MPEG, byte position 92 is 0x02 for calibrate(relative), 0x01 for verify(absolute) */
		/* byte position 92 values tested, 0x00, 0x01, 0x02, 0x03 work */
		/* Packet identifier is 0b00 i.e. 0x00, 0x00, 0x00, 0x00, as per patent https://patentimages.storage.googleapis.com/1d/96/ae/9a569883484ca7/WO2017079132A1.pdf */
		/* Data bytes are DoVi Display Management Metadata Bistream, as per patent above */
		unsigned char cal8_2[] = {0x00, 0x00, 0x00, 0x00, 0x5d, 0x00, 0x00, 0x25, 0x66, 0x00, 0x00, 0x39, 0x93, 0x25, 0x66, 0xf9, 0x27, 0xee, 
								 0xe2, 0x25, 0x66, 0x43, 0xd9, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 
								 0x00, 0x16, 0xd5, 0x25, 0xe6, 0x03, 0x45, 0x0a, 0x08, 0x2f, 0xe0, 0x06, 0x19, 0x00, 0x00, 0x02, 0xa7, 0x3d, 
								 0x59, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x01, 0x01, 0x01, 0x00, 0x3e, 0x0e, 
								 0x70, 0x00, 0x2a, 0x02, 0x00, 0x00, 0x00, 0x06, 0x01, 0x00, 0x3e, 0x0e, 0x70, 0x07, 0x57, 0x00, 0x00, 0x00, 
								 0x06, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
								 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3, 0xc2, 
								 0xac, 0x67 };


		unsigned char ver8_2[] = {0x00, 0x00, 0x00, 0x00, 0x5d, 0x00, 0x00, 0x25, 0x66, 0x00, 0x00, 0x39, 0x93, 0x25, 0x66, 0xf9, 0x27, 0xee, 
								 0xe2, 0x25, 0x66, 0x43, 0xd9, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 
								 0x00, 0x16, 0xd5, 0x25, 0xe6, 0x03, 0x45, 0x0a, 0x08, 0x2f, 0xe0, 0x06, 0x19, 0x00, 0x00, 0x02, 0xa7, 0x3d, 
								 0x59, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x01, 0x01, 0x01, 0x00, 0x3e, 0x0e, 
								 0x70, 0x00, 0x2a, 0x02, 0x00, 0x00, 0x00, 0x06, 0x01, 0x00, 0x3e, 0x0e, 0x70, 0x07, 0x57, 0x00, 0x00, 0x00, 
								 0x06, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
								 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc4, 0xf1, 
								 0xd9, 0x9e };
								 
		unsigned char cal8_1[] = {0x00, 0x00, 0x00, 0x00, 0x5d, 0x00, 0x00, 0x25, 0x66, 0x00, 0x00, 0x35, 0xea, 0x25, 0x66, 0xf9,
								  0xfc, 0xeb, 0x1c, 0x25, 0x66, 0x44, 0xca, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
						  		  0x00, 0x08, 0x00, 0x00, 0x00, 0x1c, 0x36, 0x22, 0x43, 0x01, 0x86, 0x0a, 0x5e, 0x30, 0x8e, 0x05,
								  0x14, 0x00, 0x00, 0x01, 0xa6, 0x3e, 0x5a, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x0c, 0x01, 0x01, 0x01, 0x00, 0x3e, 0x0e, 0x70, 0x00, 0x2a, 0x02, 0x00, 0x00, 0x00, 0x06,
								  0x01, 0x00, 0x02, 0x0d, 0x37, 0x03, 0x33, 0x00, 0x00, 0x00, 0x06, 0xff, 0x02, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x98, 0xbf, 0x4c, 0xad};
								
		unsigned char ver8_1[] = {0x00, 0x00, 0x00, 0x00, 0x5d, 0x00, 0x00, 0x25, 0x66, 0x00, 0x00, 0x35, 0xea, 0x25, 0x66, 0xf9,
								  0xfc, 0xeb, 0x1c, 0x25, 0x66, 0x44, 0xca, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
								  0x00, 0x08, 0x00, 0x00, 0x00, 0x1c, 0x36, 0x22, 0x43, 0x01, 0x86, 0x0a, 0x5e, 0x30, 0x8e, 0x05,
								  0x14, 0x00, 0x00, 0x01, 0xa6, 0x3e, 0x5a, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x0c, 0x01, 0x01, 0x01, 0x00, 0x3e, 0x0e, 0x70, 0x00, 0x2a, 0x06, 0x00, 0x00, 0x00, 0x06,
								  0x01, 0x00, 0x02, 0x0d, 0x37, 0x03, 0x33, 0x00, 0x00, 0x00, 0x06, 0xff, 0x01, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0xf1, 0x49, 0x03};
								  
		/* Profile 8.2 YCbCr444*/									   
		unsigned char cal8_2444[] = {0x00, 0x00, 0x00, 0x00, 0x5d, 0x00, 0x00, 0x25, 0x66, 0x00, 0x00, 0x39, 0x93, 0x25, 0x66, 0xf9, 0x27, 0xee, 
								 0xe2, 0x25, 0x66, 0x43, 0xd9, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 
								 0x00, 0x16, 0xd5, 0x25, 0xe6, 0x03, 0x45, 0x0a, 0x08, 0x2f, 0xe0, 0x06, 0x19, 0x00, 0x00, 0x02, 0xa7, 0x3d, 
								 0x59, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x01, 0x02, 0x01, 0x00, 0x3e, 0x0e, 
								 0x70, 0x00, 0x2a, 0x02, 0x00, 0x00, 0x00, 0x06, 0x01, 0x00, 0x3e, 0x0e, 0x70, 0x07, 0x57, 0x00, 0x00, 0x00, 
								 0x06, 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
								 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x82, 0xf3, 
								 0x62, 0x99 };
								 
		/* Profile 5 YCbCr444*/		 
		unsigned char cal5_422[] = {0x00, 0x00, 0x00, 0x00, 0x5d, 0x00, 0x00, 0x20, 0x00, 0x03, 0x1f, 0x06, 0x91, 0x20, 0x00, 0xfc,
								  0x5b, 0x04, 0x43, 0x20, 0x00, 0x01, 0x0b, 0xea, 0x57, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
								  0x00, 0x08, 0x00, 0x00, 0x00, 0x42, 0xb9, 0xfe, 0xa3, 0xfe, 0xa3, 0xfe, 0xa3, 0x42, 0xb9, 0xfe,
								  0xa3, 0xfe, 0xa3, 0xfe, 0xa3, 0x42, 0xb9, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x0c, 0x02, 0x00, 0x01, 0x00, 0x3e, 0x0e, 0x70, 0x00, 0x2a, 0x02, 0x00, 0x00, 0x00, 0x06,
								  0x01, 0x00, 0x02, 0x0d, 0x37, 0x03, 0x33, 0x00, 0x00, 0x00, 0x06, 0xff, 0x02, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf1, 0xc6, 0x14, 0x00 };

		unsigned char mask = 1; // Bit mask
		unsigned char bits[8];
		unsigned char total_bits[1024] = {0};
		int i, j = CHAR_BIT-1;

		unsigned char cal[128];
		unsigned char ver[128];
		
		int n = sizeof(cal)/sizeof(cal[0]);
		
		if (ofxRPI4Window::avi_info.colorimetry == 2) {
			memcpy(cal, cal8_2, sizeof(cal)); 
			memcpy(ver, ver8_2, sizeof(ver));
		}
		if (ofxRPI4Window::avi_info.colorimetry == 9) {
			memcpy(cal, cal8_1, sizeof(cal));
			memcpy(ver, ver8_1, sizeof(ver));
		}	
		if (ofxRPI4Window::dv_metadata == 0) {
			// Extract the bits
			for (int k=0; k < n; k++) {
//				printf("byte 0x%02x : ",cal[k]);
				for ( i = 0; i < 8; i++,j--,mask = 1) {
				// Mask each bit in the byte and store it
					bits[i] =( cal[k] & (mask<<=j))  !=0;

//					printf("%d", bits[i]);
					total_bits[num_bits] = bits[i];
					num_bits++;
				}

		//		puts("");
				j = CHAR_BIT-1;

			}
		    printf("Total number of Cal bits %d\n",num_bits);
		} else if (ofxRPI4Window::dv_metadata == 1) {
			// Extract the bits
			for (int k=0; k < n; k++) {
//				printf("byte 0x%02x : ",ver[k]);
				for ( i = 0; i < 8; i++,j--,mask = 1) {
				// Mask each bit in the byte and store it
					bits[i] =( ver[k] & (mask<<=j))  !=0;
//
//					printf("%d", bits[i]);
					total_bits[num_bits] = bits[i];
					num_bits++;
				}

	//			puts("");
				j = CHAR_BIT-1;

			}
			printf("Total number of Ver bits %d\n",num_bits);
		}
		/* Inject DoVi RPU Display Management Data */
		int z=0;
		int cycles=0;
		int shift = bit_depth - 8;
		int index;

		for (int y=0; y<height; y++) {

			for (int x=0; x<width; x++) {
        
				//Read pixel (x,y) color components 
				
				if (bit_depth == 10) index = channels/2 * (x + width * y);
				else index = channels * (x + width * y);
				if (bit_depth == 10){
					Y = short_pixels[ index ];
					Cb = short_pixels[ index + 1 ];
					Cr = short_pixels[ index + 2 ];
				} else {
					Y = pixels[ index ];
					Cb = pixels[ index + 1 ];
					Cr = pixels[ index + 2 ];	
				}
			if (cycles < 3) {
				if (z == 1024) {
					z = 0;
					cycles++;
				}				
		//		printf("posx=%d posy=%d z=%d Before: Y %d ,Cb %d , Cr %d ",x, y, z, Y, Cb, Cr);
				if (bit_depth == 10){
					Cr = 1016;
					Cb = 0xff80; 
				} else {
					Y = 128;// << shift;
					Cb = 16;// << shift;
				}
				if (total_bits[z] == 0) {
					if (bit_depth == 10) {
						Y = 0x1000;
//				Cb= 0;
					} else {
						Cr = 0;
					}
//				printf(".");
				}
				if (total_bits[z] == 1) {
					if (bit_depth == 10) {
						Y = 0x1010;
//				Cb = 1016;
					} else {
						Cr = 16;// << shift;
					}
//				printf("^");								
				}
				if (bit_depth == 10){
					short_pixels[ index ] = Y;
					short_pixels[ index + 1 ] = Cb;
					short_pixels[ index + 2] = Cr;
				} else {
					pixels[ index ] = Y;
					pixels[ index + 1 ] = Cb;
					pixels[ index + 2] = Cr;
				}
//				printf("==> After: Y %d ,Cb %d , Cr %d \n",Y, Cb, Cr);
			 	z++;	
			 }
 			if (cycles == 3) break;
			}
			if (cycles == 3) break; 
		}
		if (bit_depth == 10) {
			short_img.clear();
			ofSet10bitColor(1023,1023,1023,1023); 
			short_img.allocate(ofGetWindowWidth(),ofGetWindowHeight(), OF_IMAGE_COLOR_ALPHA);
			short_img.setFromPixels(short_pix);
		//	short_img.save("/tmp/test.png");
			ofDisableTextureEdgeHack();
			short_img.getTexture().setTextureMinMagFilter(GL_NEAREST,GL_NEAREST);
			if( short_img.isAllocated() ){ 
				short_img.draw(0,0,ofGetWindowWidth(),ofGetWindowHeight());
			}

		} else {
			img.clear();

			ofSetColor(255,255,255,255);
			img.allocate(ofGetWindowWidth(),ofGetWindowHeight(), OF_IMAGE_COLOR_ALPHA);
			img.setFromPixels(pix);
			ofDisableTextureEdgeHack();
			img.getTexture().setTextureMinMagFilter(GL_NEAREST,GL_NEAREST);
			if( img.isAllocated() ){
				img.draw(0,0,ofGetWindowWidth(),ofGetWindowHeight());
			} 

		}
ofApp::crc32mpeg(cal5_422,124);
}

void ofApp::dovi_dump() {
		int Y=0, Cb=0, Cr=0;
		//Getting pointer to pixel array of image
		unsigned char *pixels = img.getPixels().getData();
		//Calculate number of pixel components
		int width = img.getPixels().getWidth();
		int height = img.getPixels().getHeight();
		int channels = img.getPixels().getNumChannels();
		int num_bits=0;
		int hex_num=0;

		unsigned char mask = 1; // Bit mask
		unsigned char bits[8];
		unsigned char total_y0_bits[1920] = {0};
		unsigned char total_y1_bits[1920] = {0};
//		int i, j = CHAR_BIT-1;
//		int n = sizeof(byte)/sizeof(byte[0]);

		for (int y=0; y<height; y++) {
			num_bits=0;
			for (int x=0; x<width; x++) {
        
				//Read pixel (x,y) color components
				int index = channels * (x + width * y);
					num_bits++;
					Y = pixels[ index ];
					Cb = pixels[ index + 1 ];
					Cr = pixels[ index + 2 ];
					if (y==0) { 
						printf("posx=%d posy=%d  Before: Y %d ,Cb %d , Cr %d \n",x, y, Y, Cb, Cr);
						
						if (Cr == 16) {
						  total_y0_bits[x] = 1;
						} 
						if (Cr == 0) {
						  total_y0_bits[x] = 0;
						}

					}
					if (y==1) { 
						printf("posx=%d posy=%d  Before: Y %d ,Cb %d , Cr %d \n",x, y, Y, Cb, Cr);
						
						if (Cr == 16) {
						  total_y1_bits[x] = 1;
						} 
						if (Cr == 0) {
						  total_y1_bits[x] = 0;
						}

					}
					

			}	
		}
		printf("Y0 ");
		for ( int x=0; x < num_bits;x+=8) {
			for (int i=0; i < 8; i++) {
			   
				hex_num = (hex_num<<1)|total_y0_bits[i+x];
			   

			}
			printf("0x%02x, ", hex_num);
			hex_num=0;

		}
		puts("");
		printf("Y1 ");
		for ( int x=0; x < num_bits;x+=8) {
			for (int i=0;i < 8; i++) {
				hex_num = (hex_num<<1)|total_y1_bits[i+x];

			}
			printf("0x%02x, ", hex_num);
			hex_num=0;


		}
		puts("");

}

/*
 ##########################################################
 #                   Set DoVi Background                  #
 ##########################################################
*/
void ofApp::setDoViBackground(int redbg, int greenbg, int bluebg) {
	int bits = ofxRPI4Window::bit_depth;
	redbg   *= ((pow(2,(8+(bits-8))) - 1) / (pow(2,8) - 1));
	greenbg *= ((pow(2,(8+(bits-8))) - 1) / (pow(2,8) - 1));
	bluebg  *= ((pow(2,(8+(bits-8))) - 1) / (pow(2,8) - 1));
	ofApp::setColor(redbg,greenbg,bluebg);
//	fbo.begin();
	ofApp::shader_begin(0);

	ofDrawRectangle(0,0,ofGetWindowWidth(),ofGetWindowHeight());

	ofApp::shader_end(0);
//	fbo.end();
}

/*
 ##########################################################
 #                        FBO Allocate                    #
 ##########################################################
*/
void ofApp::fbo_allocate() {
	if (ofxRPI4Window::is_std_DoVi && ofxRPI4Window::bit_depth == 10) {  
  fbo10.clear();

		fbo10.allocate(ofGetWindowWidth(),ofGetWindowHeight(), GL_RGB10_A2);
		fbo10.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
		fbo10.begin();
		ofClear10bit(0,0,0,0);
		fbo10.end();
   } 
   if (ofxRPI4Window::is_std_DoVi && ofxRPI4Window::bit_depth == 8) {  
   
fbo8.clear();

		fbo8.allocate(ofGetWindowWidth(),ofGetWindowHeight(), GL_RGBA);
		fbo8.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);

		fbo8.begin();
		ofClear(0,0,0,0);
		fbo8.end();
   }
}

unsigned int ofApp::crc32mpeg(unsigned char *message, size_t l)
{
   size_t i, j;
   unsigned int crc, msb;

   crc = 0xFFFFFFFF;
   for(i = 0; i < l; i++) {
      // xor next byte to upper bits of crc
      crc ^= (((unsigned int)message[i])<<24);
      for (j = 0; j < 8; j++) {    // Do eight times.
            msb = crc>>31;
            crc <<= 1;
            crc ^= (0 - msb) & 0x04C11DB7;
      }

   }
#if 0   
   printf("crc %x ", crc>>24);
   printf("crc %x ", (crc>>16)&0xff);
   printf("crc %x ", (crc>>8)&0xff);
   printf("crc %x\n", crc&0xff);
#endif 

  return crc;         // don't complement crc on output
}