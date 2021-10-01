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
 
#include "ofMain.h"
#include "ofApp.h"

int main(int argc, char **argv){
 int w=4096;
 int h=2160;
 int ok=0;
 std::string str;
 /* Check Distro */
 if(boost::filesystem::exists("/etc/BiasiLinux/system_info"))       ok++;
 if(boost::filesystem::exists("/etc/BiasiLinux/packages.conf"))     ok++;
 if(boost::filesystem::exists("/etc/BiasiLinux/boot_device.conf"))  ok++;
 if(boost::filesystem::exists("/var/lib/BiasiLinux/PGenerator"))    ok++;
 if(boost::filesystem::exists("/var/lib/BiasiLinux/linux"))         ok++;
 if(boost::filesystem::exists("/usr/bin/pkg"))                      ok++;
 if(boost::filesystem::exists("/usr/bin/rcset"))                    ok++;
 if(boost::filesystem::exists("/usr/bin/bootloader"))               ok++;
 if(boost::filesystem::exists("/proc/device-tree/model"))           ok++;
 if(ok != 9 ) {
  printf("\nOnly on Distro BiasiLinux with PGenerator installed can be executed this program!\n\n");
  exit(1);
 }
 /* Check Device */
 ok=0;
 std::ifstream file("/proc/device-tree/model");
 std::getline(file, str);
 file.close();
 if (str.find("Raspberry") == string::npos) {
  printf("\nOnly on Raspberry Device can be executed this program!\n\n");
  exit(1);
 } 
 /* Continue */
 ofSetLogLevel(OF_LOG_VERBOSE);

 if(argc == 3) {
  w=atoi(argv[1]);
  h=atoi(argv[2]);
 }
#ifdef USE_X
 ofSetupOpenGL(w,h, OF_FULLSCREEN);
 ofRunApp( new ofApp());
#endif
    ofGLESWindowSettings settings;
    settings.setSize(w, h);
    settings.setGLESVersion(3);

    auto window = std::make_shared<ofxRPI4Window>(settings);
    auto hdr = std::make_shared<ofxTinyEXR>();
    auto app = std::make_shared<ofApp>();
    ofRunApp(window, app);
	// image_local();
    ofRunMainLoop();

} 
