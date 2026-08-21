@import AudioToolbox;

#include "pch.h"
#include "mtl.h"

@interface POCViewController : UIViewController
@end
@implementation POCViewController
- (BOOL)canBecomeFirstResponder {
  return YES;
}

- (void)touchesBegan:(NSSet<UITouch *> *) touches withEvent:(UIEvent *) event {
  CGPoint p = [[touches anyObject] locationInView:[self view]];
  glu_mouse_move(p.x, p.y);
  glu_mouse_down(p.x, p.y);
}
- (void)touchesCancelled:(NSSet<UITouch *> *) touches withEvent:(UIEvent *) event {
  CGPoint p = [[touches anyObject] locationInView:[self view]];
  glu_mouse_up(p.x, p.y);
}
- (void)touchesEnded:(NSSet<UITouch *> *) touches withEvent:(UIEvent *) event {
  CGPoint p = [[touches anyObject] locationInView:[self view]];
  glu_mouse_up(p.x, p.y);
}
- (void)touchesMoved:(NSSet<UITouch *> *) touches withEvent:(UIEvent *) event {
  CGPoint p = [[touches anyObject] locationInView:[self view]];
  glu_mouse_move(p.x, p.y);
}
@end

@interface POCWindowSceneDelegate : NSObject<UIWindowSceneDelegate>
@property (nonatomic, strong) UIWindow * window;
@end
@implementation POCWindowSceneDelegate
- (void)swipeLeft   { glu_move(-1,  0); }
- (void)swipeRight  { glu_move( 1,  0); }
- (void)swipeTop    { glu_move( 0, -1); }
- (void)swipeBottom { glu_move( 0,  1); }

- (void) scene:(UIScene *) scene willConnectToSession:(UISceneSession *) session options:(UISceneConnectionOptions *) connectionOptions
{
  POCViewController * vc = [POCViewController new];
  vc.view = [POCViewDelegate new];

  UIWindowScene * windowScene = (UIWindowScene *)scene;

  UISwipeGestureRecognizer * left = [UISwipeGestureRecognizer new];
  left.direction = UISwipeGestureRecognizerDirectionLeft;
  left.cancelsTouchesInView = NO;
  [left addTarget:self action:@selector(swipeLeft)];
  [vc.view addGestureRecognizer:left];

  UISwipeGestureRecognizer * right = [UISwipeGestureRecognizer new];
  right.direction = UISwipeGestureRecognizerDirectionRight;
  right.cancelsTouchesInView = NO;
  [right addTarget:self action:@selector(swipeRight)];
  [vc.view addGestureRecognizer:right];

  UISwipeGestureRecognizer * top = [UISwipeGestureRecognizer new];
  top.direction = UISwipeGestureRecognizerDirectionUp;
  top.cancelsTouchesInView = NO;
  [top addTarget:self action:@selector(swipeTop)];
  [vc.view addGestureRecognizer:top];

  UISwipeGestureRecognizer * bottom = [UISwipeGestureRecognizer new];
  bottom.direction = UISwipeGestureRecognizerDirectionDown;
  bottom.cancelsTouchesInView = NO;
  [bottom addTarget:self action:@selector(swipeBottom)];
  [vc.view addGestureRecognizer:bottom];

  self.window = [[UIWindow alloc] initWithWindowScene:windowScene];
  self.window.rootViewController = vc;
  [self.window makeKeyAndVisible];
}
@end

@interface POCAppDelegate : NSObject<UIApplicationDelegate>
@end
@implementation POCAppDelegate
- (UISceneConfiguration *) application:(UIApplication *) application 
configurationForConnectingSceneSession:(UISceneSession *) connectingSceneSession 
                               options:(UISceneConnectionOptions *) options
{
  UISceneConfiguration * res = [[UISceneConfiguration alloc] initWithName:@"Default"
                                                              sessionRole:connectingSceneSession.role];
  res.sceneClass = [UIWindowScene class];
  res.delegateClass = [POCWindowSceneDelegate class];
  return res;
}

- (void)applicationWillTerminate:(UIApplication *)app {
  glu_deinit();
}
@end

void sfx_save_prefs() {
  CFPropertyListRef value = sfx_enabled() ? kCFBooleanTrue : kCFBooleanFalse;
  CFPreferencesSetAppValue(CFSTR("sound"), value, kCFPreferencesCurrentApplication);
  CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
}

void sav_get_path(char * buf, unsigned buf_sz) {
  NSArray * arr = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
  NSString * dir = [arr firstObject];
  [[NSFileManager defaultManager] createDirectoryAtPath:dir
                            withIntermediateDirectories:YES
                                             attributes:nil
                                                  error:nil];
  strncpy(buf, dir.UTF8String, buf_sz);
}

int main(int argc, char ** argv) {
  @autoreleasepool {
    return UIApplicationMain(argc, argv, nil, @"POCAppDelegate");
  }
}
