@import MetalKit;

static id<MTLLibrary> load_library(id<MTLDevice> device, NSString * name) {
  NSString * path = [[NSBundle mainBundle] pathForResource:name ofType:@"metal"];
  NSString * src = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
  MTLCompileOptions * opts = [MTLCompileOptions new];
  NSError * err;
  id<MTLLibrary> lib = [device newLibraryWithSource:src options:opts error:&err];
  if (err) {
    NSLog(@"Error compiling shader: %@", err);
    return nil;
  }
  return lib;
}

@interface POCViewDelegate : MTKView<MTKViewDelegate>
@property (nonatomic,strong) id<MTLCommandQueue> queue;
@property (nonatomic,strong) id<MTLRenderPipelineState> pipeline;
@property (nonatomic,strong) id<MTLTexture> texture;
@property (nonatomic,strong) id<MTLSamplerState> sampler;
+ (id)newWithDevice:(id<MTLDevice>)device;
@end
@implementation POCViewDelegate
+ (id)newWithDevice:(id<MTLDevice>)device {
  POCViewDelegate * d = [POCViewDelegate new];
  d.device = device;
  d.queue = [device newCommandQueue];

  MTLTextureDescriptor * td = [MTLTextureDescriptor new];
  td.pixelFormat = MTLPixelFormatR8Unorm;
  td.width       = 128;
  td.height      = 32;
  btd_texture = d.texture = [device newTextureWithDescriptor:td];
  btd_load();

  MTLSamplerDescriptor * sd = [MTLSamplerDescriptor new];
  sd.minFilter = sd.magFilter = MTLSamplerMinMagFilterNearest;
  d.sampler = [device newSamplerStateWithDescriptor:sd];

  id<MTLLibrary> vert = load_library(device, @"bited.vert");
  id<MTLLibrary> frag = load_library(device, @"bited.frag");
  if (!vert || !frag) return nil;

  MTLRenderPipelineDescriptor * pd = [MTLRenderPipelineDescriptor new];
  pd.vertexFunction   = [vert newFunctionWithName:@"main0"];
  pd.fragmentFunction = [frag newFunctionWithName:@"main0"];
  pd.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
  NSError * err;
  d.pipeline = [device newRenderPipelineStateWithDescriptor:pd error:&err];
  if (err) return (NSLog(@"Error creating pipeline: %@", err), nil);

  return d;
}

- (BOOL)acceptsFirstResponder {
  return YES;
}
- (void)keyDown:(NSEvent *)event {
  NSString * chrs = event.charactersIgnoringModifiers;
  if (chrs.length != 1) return;

  unichar c = [chrs characterAtIndex:0];
  switch (c) {
    case NSLeftArrowFunctionKey:  return vlk_cursor(-1,  0);
    case NSRightArrowFunctionKey: return vlk_cursor( 1,  0);
    case NSUpArrowFunctionKey:    return vlk_cursor( 0, -1);
    case NSDownArrowFunctionKey:  return vlk_cursor( 0,  1);

    //case ' ': return vlk_toggle();

    case 'b': return vlk_box();
    case 'p': return vlk_player();
    case 't': return vlk_target();
    case 'w': return vlk_wall();
    case ' ': return vlk_empty();

    case '[': return vlk_load_prev_level();
    case ']': return vlk_load_next_level();
  }
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
}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app {
  return YES;
}
- (void)save {
  vlk_save();
}
@end

static void run() {
  id<MTLDevice> device = MTLCreateSystemDefaultDevice();

  POCViewDelegate * v = [POCViewDelegate newWithDevice:device];
  v.delegate = v;

  POCViewController * vc = [POCViewController new];
  vc.view = v;

  NSWindow * w = [NSWindow new];
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
        initWithTitle:@"Save"
               action:@selector(save)
        keyEquivalent:@"s"]];
  [menu       addItem:[[NSMenuItem alloc]
        initWithTitle:@"Quit Maped"
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
