
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda Pérez
//  Modus - C++ Music Library
//
//  Sample application
//
//  --- ViewController.h ---
//
//////////////////////////////////////////////////////////////////


#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>


//
// set frames per seconds
//
#define FPS 60


//
// set number of scenes
//
#define NUM_SCENES 1


//
// set supported orientations
//
//#define PORTRAIT_UP
//#define PORTRAIT_DOWN
#define LANDSCAPE_HOME_LEFT
#define LANDSCAPE_HOME_RIGHT


//
// set number of fingers
//
#define MAX_FINGERS 5


//
// accelerometer settings
//
//#define USE_ACCELEROMETER
//#define ACCELEROMETER_UPDATE 0.05


@interface ViewController : GLKViewController

@end
