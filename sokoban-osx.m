@import AppKit;
@import AudioUnit;
@import MetalKit;

#include "mtl.h"

@interface POCWindow : NSWindow
@end
@implementation POCWindow
- (void)keyDown:(NSEvent *)event {
  NSString * chrs = event.charactersIgnoringModifiers;
  if (chrs.length != 1) return;

  //unichar c = [chrs characterAtIndex:0];
  //switch (c) {
  //  case NSLeftArrowFunctionKey:  return gme_move(-1,  0);
  //  case NSRightArrowFunctionKey: return gme_move( 1,  0);
  //  case NSUpArrowFunctionKey:    return gme_move( 0, -1);
  //  case NSDownArrowFunctionKey:  return gme_move( 0,  1);
  //}
}
- (void) mouseDown:(NSEvent *)event {
  //CGPoint liw = [event locationInWindow];
  //CGPoint p = [self convertPoint:liw fromView:nil];
  //mu_input_mousedown(&mui_ctx, p.x, self.frame.size.height - p.y, 1);
}
- (void) mouseUp:(NSEvent *)event {
  //CGPoint liw = [event locationInWindow];
  //CGPoint p = [self convertPoint:liw fromView:nil];
  //mu_input_mouseup(&mui_ctx, p.x, self.frame.size.height - p.y, 1);
}
- (void) mouseMoved:(NSEvent *)event {
  //CGPoint liw = [event locationInWindow];
  //CGPoint p = [self convertPoint:liw fromView:nil];
  //mu_input_mousemove(&mui_ctx, p.x, self.frame.size.height - p.y);
}
- (void) mouseDragged:(NSEvent *)event {
  //CGPoint liw = [event locationInWindow];
  //CGPoint p = [self convertPoint:liw fromView:nil];
  //mu_input_mousemove(&mui_ctx, p.x, self.frame.size.height - p.y);
}
@end

@interface POCViewController : NSViewController
@end
@implementation POCViewController
@end

@interface POCAppDelegate : NSObject<NSApplicationDelegate>
@end
@implementation POCAppDelegate
- (void)applicationWillTerminate:(NSApplication *)app {
  // vlk_deinit();
}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app {
  return YES;
}
@end

void sav_get_path(char * buf, unsigned buf_sz) {
  NSArray * arr = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
  NSString * dir = [arr firstObject];
  [[NSFileManager defaultManager] createDirectoryAtPath:dir
                            withIntermediateDirectories:YES
                                             attributes:nil
                                                  error:nil];
  strncpy(buf, dir.UTF8String, buf_sz);
}

void sfx_save_prefs() {
  CFPropertyListRef value = sfx_enabled() ? kCFBooleanTrue : kCFBooleanFalse;
  CFPreferencesSetAppValue(CFSTR("sound"), value, kCFPreferencesCurrentApplication);
  CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
}

static void run() {
  POCViewController * vc = [POCViewController new];
  vc.view = [POCViewDelegate new];

  NSWindow * w = [NSWindow new];
  w.acceptsMouseMovedEvents = YES;
  w.contentViewController = vc;
  w.styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;

  NSRect crect = NSMakeRect(0, 0, 800, 600);
  NSRect frect = [w frameRectForContentRect:crect];
  [w setFrame:frect display:YES];
  [w center];
  [w makeKeyAndOrderFront:w];

  // Apple menu
  NSMenu * menu = [NSMenu new];
  [menu       addItem:[[NSMenuItem alloc]
        initWithTitle:@"Quit Sokoban"
               action:@selector(terminate:)
        keyEquivalent:@"q"]];

  NSMenuItem * item = [NSMenuItem new];
  item.submenu = menu;

  NSMenu * bar = [NSMenu new];
  [bar addItem:item];

  NSApplication * a = [NSApplication sharedApplication];
  a.delegate = [POCAppDelegate new];
  a.mainMenu = bar;
  [a activateIgnoringOtherApps:YES];
  [a run];
}

int main() {
  @autoreleasepool {
    run();
  }
}
