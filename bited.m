@import Metal;
@import MetalKit;

typedef struct btd_upc {
  int x, y;
} btd_upc_t;
static btd_upc_t btd_pc;

void btd_cursor(int dx, int dy) {
  int x = btd_pc.x + dx;
  if (x >= 0 && x < 128) btd_pc.x = x;

  int y = btd_pc.y + dy;
  if (y >= 0 && y < 128) btd_pc.y = y;
}

static uint8_t btd_atlas[128 * 32];
static id<MTLTexture> btd_texture;

void btd_replace_atlas() {
  MTLRegion r = { {0,0,0}, {128,32,1} };
  [btd_texture replaceRegion:r mipmapLevel:0 withBytes:btd_atlas bytesPerRow:128];
}

void btd_toggle() {
  int i = btd_pc.y * 128 + btd_pc.x;
  btd_atlas[i] = btd_atlas[i] ? 0 : 255;
  btd_replace_atlas();
}

void btd_load() {
  FILE * f = fopen("atlas.img", "rb");
  fread(btd_atlas, 128 * 32, 1, f);
  fclose(f);
  btd_replace_atlas();
}
void btd_save() {
  FILE * f = fopen("atlas.img", "wb");
  fwrite(btd_atlas, 128 * 32, 1, f);
  fclose(f);
}

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
- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
}
- (void)drawInMTKView:(MTKView *)view {
  MTLRenderPassDescriptor * rpd = view.currentRenderPassDescriptor;
  if (rpd == nil) return;

  id<MTLCommandBuffer> cb = [self.queue commandBuffer];

  id<MTLRenderCommandEncoder> enc = [cb renderCommandEncoderWithDescriptor:rpd];
  [enc setRenderPipelineState:self.pipeline];
  [enc setVertexBytes:&btd_pc length:sizeof(btd_upc_t) atIndex:0];
  [enc setFragmentBytes:&btd_pc length:sizeof(btd_upc_t) atIndex:0];
  [enc setFragmentTexture:self.texture atIndex:0];
  [enc setFragmentSamplerState:self.sampler atIndex:0];
  [enc drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
  [enc endEncoding];

  [cb presentDrawable:view.currentDrawable];
  [cb commit];
}

- (BOOL)acceptsFirstResponder {
  return YES;
}
- (void)keyDown:(NSEvent *)event {
  NSString * chrs = event.charactersIgnoringModifiers;
  if (chrs.length != 1) return;

  unichar c = [chrs characterAtIndex:0];
  switch (c) {
    case NSLeftArrowFunctionKey:  return btd_cursor(-1,  0);
    case NSRightArrowFunctionKey: return btd_cursor( 1,  0);
    case NSUpArrowFunctionKey:    return btd_cursor( 0, -1);
    case NSDownArrowFunctionKey:  return btd_cursor( 0,  1);

    case ' ': return btd_toggle();

    case 'e': return btd_load();
    case 'w': return btd_save();
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
  vlk_deinit();
}
- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)app {
  return YES;
}
@end

CAMetalLayer * vlk_metal_layer() {
  return (CAMetalLayer *)[NSApplication sharedApplication].windows[0].contentView.layer;
}

FILE * vlk_open(const char * name, const char * ext) {
  NSString * n = [NSString stringWithFormat:@"%s", name];
  NSString * e = [NSString stringWithFormat:@"%s", ext];
  NSString * path = [[NSBundle mainBundle] pathForResource:n ofType:e];
  return fopen(path.UTF8String, "rb");
}

void vlk_log(int r, const char * msg) {
  NSLog(@"Vulkan call failed (code=%d): %s\n", r, msg);
  exit(1);
}

static void run() {
  id<MTLDevice> device = MTLCreateSystemDefaultDevice();

  POCViewDelegate * v = [POCViewDelegate newWithDevice:device];
  v.delegate = v;

  POCViewController * vc = [POCViewController new];
  vc.view = v;

  NSWindow * w = [NSWindow new];
  w.contentViewController = vc;
  w.styleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable;

  NSRect crect = NSMakeRect(0, 0, 1024, 256);
  NSRect frect = [w frameRectForContentRect:crect];
  [w setFrame:frect display:YES];
  [w center];
  [w makeKeyAndOrderFront:w];

  // Apple menu
  NSMenu * menu = [NSMenu new];
  [menu       addItem:[[NSMenuItem alloc]
        initWithTitle:@"Quit Bited"
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
