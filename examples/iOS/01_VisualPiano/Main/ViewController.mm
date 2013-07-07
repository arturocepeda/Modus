
//////////////////////////////////////////////////////////////////
//
//  Arturo Cepeda Pérez
//  Modus - C++ Music Library
//
//  Sample application
//
//  --- ViewController.mm ---
//
//////////////////////////////////////////////////////////////////


#import "ViewController.h"

#import "GERendering.h"
#import "GEScene.h"

#import "SceneSample.h"


@interface ViewController () <UIAccelerometerDelegate>
{
   GLuint _program;
   
   // Rendering system
   GERendering* cRender;
   
   // Scene management
   GEScene* cScenes[NUM_SCENES];
   int iCurrentScene;
   
   // Input management
   int iFingerID[MAX_FINGERS];
   UIAccelerometer* uiAccel;
}

@property (strong, nonatomic) EAGLContext* context;
@property (strong, nonatomic) GLKBaseEffect* effect;

-(void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration;
-(void) selectScene:(unsigned int) iScene;

@end


@implementation ViewController

@synthesize context = _context;
@synthesize effect = _effect;

-(void) dealloc
{
    [_context release];
    [_effect release];
    [super dealloc];
}

-(void) viewDidLoad
{
   [super viewDidLoad];
    
   self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];

   if(!self.context) 
       NSLog(@"Failed to create ES context");
    
   GLKView* view = (GLKView*)self.view;
   view.context = self.context;
   view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
   
   // init OpenGL
   [EAGLContext setCurrentContext:self.context];
   
   // set frames per second
   self.preferredFramesPerSecond = FPS;
   
   // enable multiple touch
   self.view.userInteractionEnabled = YES;
   self.view.multipleTouchEnabled = YES;
   self.view.exclusiveTouch = YES;
   
   // IDs for touch management
   for(int i = 0; i < MAX_FINGERS; i++)
      iFingerID[i] = 0;
   
#ifdef USE_ACCELEROMETER
   // accelerometer
   uiAccel = [UIAccelerometer sharedAccelerometer];
   uiAccel.updateInterval = ACCELEROMETER_UPDATE;
   uiAccel.delegate = self;
#endif
   
   // initialize rendering system
   cRender = new GERendering(self.context);
   
   // create scenes
   cScenes[0] = (GEScene*)new GESceneSample(cRender, false);
   // ...
   // ...
   
   // select the first scene   
   iCurrentScene = 0;
   cScenes[0]->init();
}

-(void) viewDidUnload
{    
   [super viewDidUnload];
   
   [EAGLContext setCurrentContext:self.context];
   
   if(_program) 
   {
      glDeleteProgram(_program);
      _program = 0;
   }
    
   if([EAGLContext currentContext] == self.context) 
      [EAGLContext setCurrentContext:nil];

   self.context = nil;
}

-(void) didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

-(BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
#ifdef PORTRAIT_UP
   if(interfaceOrientation == UIDeviceOrientationPortrait)
      return YES;
#endif
#ifdef PORTRAIT_DOWN
   if(interfaceOrientation == UIDeviceOrientationPortraitUpsideDown)
      return YES;
#endif
#ifdef LANDSCAPE_HOME_LEFT
   if(interfaceOrientation == UIDeviceOrientationLandscapeRight)
      return YES;
#endif
#ifdef LANDSCAPE_HOME_RIGHT
   if(interfaceOrientation == UIDeviceOrientationLandscapeLeft)
      return YES;
#endif   
   
   return NO;
}

-(void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration
{
   cScenes[iCurrentScene]->updateAccelerometerStatus(acceleration.x, acceleration.y, acceleration.z);
}

-(void) selectScene:(unsigned int)iScene
{
   cScenes[iCurrentScene]->release();
   iCurrentScene = iScene;
   cScenes[iCurrentScene]->init();
}

-(void) update
{
   cScenes[iCurrentScene]->update();
    
   // scene change request
   if(cScenes[iCurrentScene]->getNextScene() >= 0)
      [self selectScene:cScenes[iCurrentScene]->getNextScene()];
}

-(void) glkView:(GLKView *)view drawInRect:(CGRect)rect
{
   cRender->renderBegin();
   cScenes[iCurrentScene]->render();
   cRender->renderEnd();
}

-(void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
   NSArray* uiTouchesList = [touches allObjects];
   
   for(UITouch* uiTouch in uiTouchesList)
   {
      for(int i = 0; i < MAX_FINGERS; i++)
      {
         if(iFingerID[i] == 0)
         {
            CGPoint cgPoint = [uiTouch locationInView: self.view];
            
            iFingerID[i] = (int)uiTouch;
            cScenes[iCurrentScene]->inputTouchBegin(i, &cgPoint);
            
            break;
         }
      }
   }
}

-(void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
   NSArray* uiTouchesList = [touches allObjects];   
   
   for(UITouch* uiTouch in uiTouchesList)
   {
      for(int i = 0; i < MAX_FINGERS; i++)
      {
         if(iFingerID[i] == (int)uiTouch)
         {
            CGPoint cgPreviousPoint = [uiTouch previousLocationInView: self.view];
            CGPoint cgCurrentPoint = [uiTouch locationInView: self.view];
            
            cScenes[iCurrentScene]->inputTouchMove(i, &cgPreviousPoint, &cgCurrentPoint);
            
            break;
         }
      }
   }   
}

-(void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
   NSArray* uiTouchesList = [touches allObjects];
   
   for(UITouch* uiTouch in uiTouchesList)
   {
      for(int i = 0; i < MAX_FINGERS; i++)
      {
         if(iFingerID[i] == (int)uiTouch)
         {
            CGPoint cgPoint = [uiTouch locationInView: self.view];
            
            iFingerID[i] = 0;
            cScenes[iCurrentScene]->inputTouchEnd(i, &cgPoint);
            
            break;
         }
      }
   }
}

-(void) touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
   [self touchesEnded:touches withEvent:event];
}

@end
