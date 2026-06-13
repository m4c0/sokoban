#include "gme.h"
#include "mui.h"
#include "sfx.h"
#include "vlk-sokoban.h"

#include <knownfolders.h>
#include <shlobj.h>

HWND vlk_hwnd;

FILE * vlk_open(const char * name, const char * ext) {
  char exe[MAX_PATH];
  GetModuleFileName(NULL, exe, MAX_PATH);

  char * p = strrchr(exe, '\\');
  if (p) *p = 0;

  char buf[MAX_PATH]; snprintf(buf, MAX_PATH, "%s\\%s.%s", exe, name, ext);
  return fopen(buf, "rb");
}

static char vlk_log_buf[1024];
void vlk_log(int r, const char * msg) {
  snprintf(vlk_log_buf, 1024, "Vulkan call failed (code=%d): %s\n", r, msg);
  MessageBox(NULL, vlk_log_buf, "Vulkan error", 0);

  // This is the only way to properly programatically exit an app from any
  // thread. Other attempts froze the app or kept it as a "background app".
  // i.e. We use WM_CLOSE instead of WM_QUIT (or PostQuitMessage etc) and we
  // need to use SendNotifyMessage instead of SendMessage.
  if (vlk_hwnd) SendNotifyMessage(vlk_hwnd, WM_CLOSE, 0, 0);
}

void sav_get_path(char * buf, unsigned sz) {
  // https://learn.microsoft.com/en-us/windows/win32/api/shlobj_core/nf-shlobj_core-shgetknownfolderpath
  // FOLDERID_SavedGames
  PWSTR sg;

  if (S_OK != SHGetKnownFolderPath(&FOLDERID_SavedGames, KF_FLAG_CREATE, 0, &sg)) {
    buf[0] = 0;
    return;
  }

  size_t count;
  wcstombs_s(&count, buf, sz, sg, sz - 1);
  CoTaskMemFree(sg);
}

#define SFX_KEY_NAME "Software\\m4c0\\sokoban"
static HKEY sfx_key_cached;
static HKEY sfx_key() {
  if (sfx_key_cached) return sfx_key_cached;

  RegCreateKeyExA(
      HKEY_CURRENT_USER, SFX_KEY_NAME, 0, NULL, 0,
      KEY_WRITE | KEY_READ, NULL, &sfx_key_cached, NULL);

  return sfx_key_cached;
}

void sfx_save_prefs() {
  uint32_t val = sfx_enabled() ? 1 : 0;
  RegSetValueExA(sfx_key(), "sound", 0, REG_DWORD, (void *)&val, sizeof(val));
}

static LRESULT window_proc(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
  switch (msg) {
    case WM_CLOSE:
      // Required to enable another thread sending "plz exit" messages
      DestroyWindow(hwnd);
      return 0;
    case WM_DESTROY:
      vlk_deinit();
      PostQuitMessage(0);
      return 0;

    case WM_MOUSEMOVE:
      mu_input_mousemove(&mui_ctx, GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param));
      return 0;
    case WM_LBUTTONDOWN:
      mu_input_mousedown(&mui_ctx, GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), 1);
      return 0;
    case WM_LBUTTONUP:
      mu_input_mouseup(&mui_ctx, GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param), 1);
      return 0;

    case WM_KEYDOWN:
      if (HIWORD(l_param) & KF_REPEAT) return 0;

      switch (LOWORD(w_param)) {
        case VK_LEFT:   gme_move(-1,  0); break;
        case VK_RIGHT:  gme_move( 1,  0); break;
        case VK_UP:     gme_move( 0, -1); break;
        case VK_DOWN:   gme_move( 0,  1); break;
      }

      return 0;

    case WM_ERASEBKGND:
      // i.e. "never erase background". Solves 99.999% of flicker issues
      return 1;
    case WM_PAINT:
      if (vlk_hwnd) vlk_frame();
      return 0;
  }
  return DefWindowProc(hwnd, msg, w_param, l_param);
}

int WinMain(HINSTANCE h_instance, HINSTANCE h_prev, LPSTR cmd_line, int cmd_show) {
  HICON h_icon = LoadIcon(h_instance, "IDI_ICON");

  WNDCLASSEX wcex  = {
    .cbSize        = sizeof(WNDCLASSEX),
    .style         = CS_HREDRAW | CS_VREDRAW,
    .lpfnWndProc   = &window_proc,
    .hInstance     = h_instance,
    .hIcon         = h_icon,
    .hCursor       = LoadCursor(NULL, IDC_ARROW),
    .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
    .lpszClassName = "m4c0-sokoban-window",
    .hIconSm       = h_icon,
  };
  if (!RegisterClassEx(&wcex)) {
    MessageBox(NULL, "Failed to register window class", "Unhandled error", 0);
    return 1;
  }

  DWORD style = WS_OVERLAPPEDWINDOW ^ WS_SIZEBOX ^ WS_MAXIMIZEBOX;

  HWND hwnd = CreateWindow(
      "m4c0-sokoban-window",
      "Casually Casual Warehouse Game",
      style, CW_USEDEFAULT, CW_USEDEFAULT,
      800, 600, 
      NULL, NULL, h_instance, NULL);
  if (!hwnd) {
    MessageBox(NULL, "Failed to create window", "Unhandled error", 0);
    return 1;
  }

  ShowWindow(hwnd, cmd_show);
  UpdateWindow(hwnd);

  uint32_t val = 1;
  DWORD size = sizeof(val);
  RegQueryValueExA(sfx_key(), "sound", NULL, NULL, (void *)&val, &size);
  sfx_init(val ? 1 : 0);

  vlk_hwnd = hwnd;
  vlk_init();

  MSG msg;
  while (GetMessage(&msg, 0, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}
