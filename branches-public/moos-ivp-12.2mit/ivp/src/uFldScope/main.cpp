/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: main.cpp                                             */
/*   DATE: Nov 23rd 2011                                        */
/****************************************************************/

#include "unistd.h"
#include "MBUtils.h"
#include "TermUtils.h"
#include "ColorParse.h"
#include "MOOSAppRunnerThread.h"
#include "UFS_MOOSApp.h"
#include "UFS_Info.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file;
  string run_command = argv[0];

  for(int i=1; i<argc; i++) {
    string argi = argv[i];
    if((argi=="-v") || (argi=="--version") || (argi=="-version"))
      showReleaseInfoAndExit();
    else if((argi=="-e") || (argi=="--example") || (argi=="-example"))
      showExampleConfigAndExit();
    else if((argi=="-h") || (argi == "--help") || (argi=="-help"))
      showHelpAndExit();
    else if((argi=="-i") || (argi == "--interface"))
      showInterfaceAndExit();
    else if(strEnds(argi, ".moos") || strEnds(argi, ".moos++"))
      mission_file = argv[i];
    else if(strBegins(argi, "--alias="))
      run_command = argi.substr(8);
    else if(i == 2)
      run_command = argi;
  }
  
  if(mission_file == "")
      showHelpAndExit();

  cout << termColor("green");
  cout << "uFldScope launching as " << run_command << endl;
  cout << termColor() << endl;

  UFS_MOOSApp ufs_moosapp;
  // start the scope in its own thread

#if 0
  ufs_moosapp.Run(run_command.c_str(), mission_file.c_str());
  return(0);
#endif


#if 1
  MOOSAppRunnerThread appRunner(&ufs_moosapp, 
				(char*)(run_command.c_str()), 
				mission_file.c_str());

  bool quit = false;
  while(!quit) {
    usleep(100000);
    char c = getCharNoWait();
    cout << "[" << c << "]" << endl;
    if((c=='q') || (c==(char)(3)))  { // ASCII 03 is control-c
      cout << "Quitting......." << flush;
      quit = true;
    }
    else
      ufs_moosapp.handleCommand(c);
  }

  appRunner.quit();
  return(0);
#endif

}

