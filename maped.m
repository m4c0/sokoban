@import MetalKit;

#include "gme.h"
#include "lvl.h"

typedef struct mpd_vec2 {
  float x, y;
} mpd_vec2_t;
typedef struct mpd_upc {
  float sel_rect_x, sel_rect_y, sel_rect_w, sel_rect_h;
  float player_pos_x, player_pos_y;
  float label_pos_x, label_pos_y;
  float cursor_x, cursor_y;
  mpd_vec2_t menu_size;
  float level;
  float aspect;
  float time;
  float back_btn_dim;
  float menu_btn_dim;
} mpd_upc_t;

static mpd_upc_t mpd_pc;
static int mpd_cur_x = LVL_WIDTH / 2;
static int mpd_cur_y = LVL_HEIGHT / 2;

static char * mpd_ptr;

void mpd_update_map();

static void mpd_load_map(int lvl) {
  lvl_load(lvl, mpd_ptr);
  mpd_update_map();
}
void mpd_load_next_level() {
  int lvl = lvl_current + 1;
  if (lvl > lvl_max_level) lvl = 0;
  mpd_load_map(lvl);
}
void mpd_load_prev_level() {
  int lvl = lvl_current - 1;
  if (lvl < 0) lvl = lvl_max_level;
  mpd_load_map(lvl);
}

void mpd_cursor(int dx, int dy) {
  mpd_cur_x += dx;
  if (mpd_cur_x < 0) mpd_cur_x = 0;
  if (mpd_cur_x >= LVL_WIDTH) mpd_cur_x = LVL_WIDTH;

  mpd_cur_y += dy;
  if (mpd_cur_y < 0) mpd_cur_y = 0;
  if (mpd_cur_y >= LVL_HEIGHT) mpd_cur_x = LVL_HEIGHT;
}

static void mpd_clear_player() {
  char * p = mpd_ptr + lvl_py * LVL_WIDTH + lvl_px;
  if (p < mpd_ptr || p >= mpd_ptr + LVL_SZ) return;
  switch (*p) {
    case gme_b_player:        *p = gme_b_empty;  break;
    case gme_b_player_target: *p = gme_b_target; break;
    default: assert(0 && "invalid map state in old player pos"); // unreachable
  }
}
static void mpd_update_player(char * p, char c) {
  mpd_clear_player();
  *p = c;        
  lvl_px = mpd_cur_x;
  lvl_py = mpd_cur_y;
}
void mpd_player() {
  char * p = mpd_ptr + mpd_cur_y * LVL_WIDTH + mpd_cur_x;
  if (p < mpd_ptr || p >= mpd_ptr + LVL_SZ) return;
  switch (*p) {
    case gme_b_empty:  mpd_update_player(p, gme_b_player);        break;
    case gme_b_target: mpd_update_player(p, gme_b_player_target); break;
  }
  mpd_update_map();
}

void mpd_empty() {
  char * p = mpd_ptr + mpd_cur_y * LVL_WIDTH + mpd_cur_x;
  if (p < mpd_ptr || p >= mpd_ptr + LVL_SZ) return;
  switch (*p) {
    case gme_b_outside: *p = gme_b_empty;   break;
    case gme_b_empty:   *p = gme_b_outside; break;
    case gme_b_wall:    *p = gme_b_empty;   break;
  }
  mpd_update_map();
}

void mpd_wall() {
  char * p = mpd_ptr + mpd_cur_y * LVL_WIDTH + mpd_cur_x;
  if (p < mpd_ptr || p >= mpd_ptr + LVL_SZ) return;
  switch (*p) {
    case gme_b_empty:
    case gme_b_outside: *p = gme_b_wall;    break;
    case gme_b_wall:    *p = gme_b_outside; break;
  }
  mpd_update_map();
}

void mpd_box() {
  char * p = mpd_ptr + mpd_cur_y * LVL_WIDTH + mpd_cur_x;
  if (p < mpd_ptr || p >= mpd_ptr + LVL_SZ) return;
  switch (*p) {
    case gme_b_outside:
    case gme_b_empty:      *p = gme_b_box;        break;
    case gme_b_box:        *p = gme_b_empty;      break;
    case gme_b_target:     *p = gme_b_target_box; break;
    case gme_b_target_box: *p = gme_b_target;     break;
  }
  mpd_update_map();
}
void mpd_target() {
  char * p = mpd_ptr + mpd_cur_y * LVL_WIDTH + mpd_cur_x;
  if (p < mpd_ptr || p >= mpd_ptr + LVL_SZ) return;
  switch (*p) {
    case gme_b_outside:
    case gme_b_empty:         *p = gme_b_target;        break;
    case gme_b_box:           *p = gme_b_target_box;    break;
    case gme_b_target:        *p = gme_b_empty;         break;
    case gme_b_target_box:    *p = gme_b_box;           break;
    case gme_b_player:        *p = gme_b_player_target; break;
    case gme_b_player_target: *p = gme_b_player;        break;
  }
  mpd_update_map();
}

void mpd_save() {
  assert(0 == fseek(lvl_f, (LVL_SZ + 3) * lvl_current + 1, SEEK_SET));
  assert(1 == fwrite(mpd_ptr, LVL_SZ, 1, lvl_f));
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
  d.texture = [device newTextureWithDescriptor:td];

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
  [enc setVertexBytes:&mpd_pc length:sizeof(mpd_upc_t) atIndex:0];
  [enc setFragmentBytes:&mpd_pc length:sizeof(mpd_upc_t) atIndex:0];
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
    case NSLeftArrowFunctionKey:  return mpd_cursor(-1,  0);
    case NSRightArrowFunctionKey: return mpd_cursor( 1,  0);
    case NSUpArrowFunctionKey:    return mpd_cursor( 0, -1);
    case NSDownArrowFunctionKey:  return mpd_cursor( 0,  1);

    //case ' ': return mpd_toggle();

    case 'b': return mpd_box();
    case 'p': return mpd_player();
    case 't': return mpd_target();
    case 'w': return mpd_wall();
    case ' ': return mpd_empty();

    case '[': return mpd_load_prev_level();
    case ']': return mpd_load_next_level();
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
  mpd_save();
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
