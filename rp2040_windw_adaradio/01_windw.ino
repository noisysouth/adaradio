// --------------------- colors ------------------------

enum color_e {
#ifdef ESP32_S3_TFT
  color_black = ST77XX_BLACK,
  color_white = ST77XX_WHITE,
  color_blue = ST77XX_BLUE,
  color_green = ST77XX_GREEN,
  color_yellow = ST77XX_YELLOW,
  color_red = ST77XX_RED,
  color_magenta = ST77XX_MAGENTA,
#elif defined HX8357
//#ifdef HX8357
  color_black = HX8357_BLACK,
  color_white = HX8357_WHITE,
  color_blue = HX8357_BLUE,
  color_green = HX8357_GREEN,
  color_yellow = HX8357_YELLOW,
  color_red = HX8357_RED,
  color_magenta = HX8357_MAGENTA,
#else
  color_black = 0x0000,
  color_white = 0xffff,
#endif
  // raw 565 format colors:
  // Source: https://github.com/newdigate/rgb565_colors/tree/main?tab=readme-ov-file#gray
  color_silver = 0xbdf7,
  //color_lt_gray = 0x6B4D, // dim gray 
  color_lt_gray = 0xd69a,
  color_dk_gray = 0x29A7, // gunmetal gray

  color_backgr = color_black,
  color_highlight = color_silver, // color_white,
  //color_shadow = 0x328A, // dark slate gray
  color_shadow = color_dk_gray,
  color_window = color_lt_gray,

  color_cap_shadow = color_black,
  color_cap_bar = color_shadow,
  color_cap_label = color_white,

  color_wndw_text = color_black,
  color_btn_text = color_black,
  color_edit_text = color_black,
  color_edit_backgr = color_white,
  color_edit_cursor = color_black,

  color_label_text = color_black,
  color_label_backgr = color_window,

  color_focus_border = color_black,

  color_count,
};

int get_color_backgr (void) {
  return color_backgr;
}

// ----------------------- window structures ----------------------------

enum border_style_e {
  border_outset = 0,
  border_ridge,
  border_outset_narrow,
  border_inset,
  border_focus,
  border_style_count,
};

struct windw_s {
	int wx, wy, ww, wh; // Window coords.
	int border_style;		//  enum border_style_e
  int buffered;		    // Flag for buffer.
  GFXcanvas16 *canvas; // 16-bit, width x height pixels
};

#define WINDW_MAX_CAP 61
#define CHAR_WIDTH_PIXELS 12
#define CHAR_HEIGHT_PIXELS 16

struct cap_windw_s {
  struct windw_s windw;
  char label[WINDW_MAX_CAP]; // full cap
};

#define WINDW_MAX_LINE 61

struct cap_twindw_s {
  struct cap_windw_s cwindw;
  char line1[WINDW_MAX_LINE];
  char line2[WINDW_MAX_LINE];
  int button;
};

#define BUTTON_WIDTH_PIXELS  64
#define BUTTON_HEIGHT_PIXELS 32
#define BUTTON_MAX_LABEL 20

struct button_s {
  struct windw_s windw;
  char label_str[BUTTON_MAX_LABEL];
	unsigned hotkey;
	int altkey;
  //GFXcanvas16 canvas(BUTTON_WIDTH_PIXELS, BUTTON_HEIGHT_PIXELS); // 16-bit, width x height pixels
  GFXcanvas16 *label_canvas; // 16-bit, width-4 x height-4 pixels
};

#define EDIT_WIDTH_PIXELS   64
#define EDIT_HEIGHT_PIXELS  32
#define EDIT_MAX_TEXT       WINDW_MAX_LINE
#define CURSOR_BLINK_COUNT  5

struct edit_s {
  struct windw_s windw;
  char text_str[EDIT_MAX_TEXT];
  int text_max_chars;
  int cursor_idx;
  bool cursor_blink_on;
  int  cursor_blink_count;
};

#define LABEL_WIDTH_PIXELS   64
#define LABEL_HEIGHT_PIXELS  32
#define LABEL_MAX_TEXT       WINDW_MAX_LINE

struct label_s {
  struct windw_s windw;
  char text_str[LABEL_MAX_TEXT];
  int text_max_chars;
};

#define LIST_MAX_TEXT   WINDW_MAX_LINE
#define LIST_MAX_ITEMS  100
// TODO: implement
struct list_s {
  struct windw_s windw;
  int scroll_idx;
  int item_count;
  char text_str[LIST_MAX_ITEMS][LIST_MAX_TEXT];
};

enum ctrl_type_e {
  ctrl_type_none = 0,
  ctrl_windw,
  ctrl_cap_windw,
  ctrl_cap_twindw,
  ctrl_button,
  ctrl_up_down, // TODO
  ctrl_edit,
  ctrl_scroll,  // TODO
  ctrl_list,    // TODO
  ctrl_type_count,
};

struct tab_ctrl_s {
  int   ctrl_type; // enum ctrl_type_e
  void *ctrl_obj;
};

#define SCREEN_MAX_CTRLS 10
struct ctrl_screen_s {
  struct tab_ctrl_s screen_ctrls[SCREEN_MAX_CTRLS];
  int               focus_idx;
};

// ----------------------- draw Dashed lines -----------------------------

#define DOT_LINE_LEN 10
#define DOT_SPACE_LEN 3
#define FOCUS_MARGIN_PIXELS 4

// to TFT
#ifdef ESP32_S3_TFT
  void DrawDashedHLine(Adafruit_ST7789 *scrn, int line_x, int line_y, int line_w) {
#elif defined HX8357
  void DrawDashedHLine(Adafruit_HX8357 *scrn, int line_x, int line_y, int line_w) {
#else
  void DrawDashedHLine(GFXcanvas16 *scrn, int line_x, int line_y, int line_w) {
#endif
    // top and left sides
    int start_px, end_px;
    int width_px;
    int limit_px;

    start_px = line_x;
    limit_px = line_x + line_w;

    width_px = DOT_LINE_LEN;
    while (start_px < limit_px) {
      end_px = start_px + width_px;
      if (end_px > limit_px) {
        end_px = limit_px;
        width_px = end_px - limit_px;
      }

      scrn->drawFastHLine(start_px, line_y, width_px, color_focus_border);
      start_px += DOT_LINE_LEN + DOT_SPACE_LEN;
    }
}

#ifdef ESP32_S3_TFT
  void DrawDashedVLine(Adafruit_ST7789 *scrn, int line_x, int line_y, int line_w) {
#elif defined HX8357
  void DrawDashedVLine(Adafruit_HX8357 *scrn, int line_x, int line_y, int line_w) {
#else
  void DrawDashedVLine(GFXcanvas16 *scrn, int line_x, int line_y, int line_w) {
#endif
    // top and left sides
    int start_py, end_py;
    int height_py;
    int limit_py;

    start_py = line_y;
    limit_py = line_y + line_w;

    height_py = DOT_LINE_LEN;
    while (start_py < limit_py) {
      end_py = start_py + height_py;
      if (end_py > limit_py) {
        end_py = limit_py;
        height_py = end_py - limit_py;
      }
      scrn->drawFastVLine(line_x, start_py, height_py, color_focus_border);
      start_py += DOT_LINE_LEN + DOT_SPACE_LEN;
    }
}

// to Canvas
void DrawDashedHLineToCanvas(GFXcanvas16 *canvas, int line_x, int line_y, int line_w) {
    // top and left sides
    int start_px, end_px;
    int width_px;
    int limit_px;

    start_px = line_x;
    limit_px = line_x + line_w;

    width_px = DOT_LINE_LEN;
    while (start_px < limit_px) {
      end_px = start_px + width_px;
      if (end_px > limit_px) {
        end_px = limit_px;
        width_px = end_px - limit_px;
      }
      canvas->drawFastHLine(start_px, line_y, width_px, color_focus_border);
      start_px += DOT_LINE_LEN + DOT_SPACE_LEN;
    }
}

void DrawDashedVLineToCanvas(GFXcanvas16 *canvas, int line_x, int line_y, int line_w) {
    // top and left sides
    int start_py, end_py;
    int height_py;
    int limit_py;

    start_py = line_y;
    limit_py = line_y + line_w;

    height_py = DOT_LINE_LEN;
    while (start_py < limit_py) {
      end_py = start_py + height_py;
      if (end_py > limit_py) {
        end_py = limit_py;
        height_py = end_py - limit_py;
      }
      canvas->drawFastVLine(line_x, start_py, height_py, color_focus_border);
      start_py += DOT_LINE_LEN + DOT_SPACE_LEN;
    }
}

// Adapted from: Borland C++ Power Programming by Clayton Walnum
// https://fabiensanglard.net/reverse_engineering_strike_commander/docs/Borland_C___Power_Programming_Book_and_Disk__Programming_.pdf
// pg. 129 (PDF pg. 143)

// ------------------- draw Border ------------------------

#ifdef ESP32_S3_TFT
  void DrawBorder(Adafruit_ST7789 *scrn, struct windw_s *wnd, int brd_style) {
#elif defined HX8357
  void DrawBorder(Adafruit_HX8357 *scrn, struct windw_s *wnd, int brd_style) {
#else
  void DrawBorder(GFXcanvas16 *scrn, struct windw_s *wnd, int brd_style) {
#endif
  int bx, by, bw, bh;

  bx = wnd->wx;
  by = wnd->wy;
  bw = wnd->ww;
  bh = wnd->wh;
  
  switch (brd_style) {
  case border_outset:
    // highlight - top and left sides, 2 pixels wide each
    scrn->drawFastHLine(bx,   by,   bw,   color_highlight); //  top edge - outer
    scrn->drawFastVLine(bx,   by,   bh,   color_highlight); // left edge - outer
    scrn->drawFastHLine(bx+1, by+1, bw-2, color_highlight); //  top edge - inner
    scrn->drawFastVLine(bx+1, by+1, bh-2, color_highlight); // left edge - innder

    // shadow - bottom and right sides, 2 pixels wide each
    scrn->drawFastHLine(bx+1,    by+bh-1, bw-1, color_shadow); // bottom edge - outer
    scrn->drawFastVLine(bx+bw-1, by,      bh,   color_shadow); //  right edge - outer
    scrn->drawFastHLine(bx+2,    by+bh-2, bw-3, color_shadow); // bottom edge - inner
    scrn->drawFastVLine(bx+bw-2, by+1,    bh-2, color_shadow); //  right edge - inner
    break;
  case border_ridge:
    scrn->drawFastHLine(bx+10,      by+10,      bw-20, color_shadow); //  top margin line
    scrn->drawFastVLine(bx+10,      by+10,      bh-20, color_shadow); // left margin line
    scrn->drawFastHLine(bx+10,      by+bh - 10, bw-20, color_highlight); // bottom margin line    
    scrn->drawFastVLine(bx+bw - 10, 10,         bh-20, color_highlight); //   left margin line
    break;
  case border_outset_narrow: // Draw caption bar
    scrn->drawFastVLine(bx + 20,      by+20, 20,      color_highlight);  //   left edge
    scrn->drawFastHLine(bx + 20,      by+20, bw - 40, color_highlight);  //    top edge
    scrn->drawFastVLine(bx + bw - 20, by+20, 21,      color_cap_shadow); //  right edge
    scrn->drawFastHLine(bx + 20,      by+40, bw - 40, color_cap_shadow); // bottom edge
    break;
  case border_inset:
    // shadow - top and left sides, 2 pixels wide each
    scrn->drawFastHLine(bx,   by,   bw-1, color_shadow); //  top edge - outer
    scrn->drawFastVLine(bx,   by,   bh-1, color_shadow); // left edge - outer
    scrn->drawFastHLine(bx+1, by+1, bw-3, color_shadow); //  top edge - inner
    scrn->drawFastVLine(bx+1, by+1, bh-3, color_shadow); // left edge - innder

    // highlight - bottom and right sides, 2 pixels wide each
    scrn->drawFastHLine(bx+1,      by + bh-1, bw-1, color_highlight); // bottom edge - outer
    scrn->drawFastVLine(bx + bw-1, by,        bh-1, color_highlight); //  right edge - outer
    scrn->drawFastHLine(bx+2,      by + bh-2, bw-3, color_highlight); // bottom edge - inner
    scrn->drawFastVLine(bx + bw-2, by+1,      bh-3, color_highlight); //  right edge - inner
    break;
  case border_focus:
    // for some reason using the scrn parameter does not work here?
    DrawDashedHLine(scrn, bx+FOCUS_MARGIN_PIXELS, by+FOCUS_MARGIN_PIXELS,      bw-FOCUS_MARGIN_PIXELS*2); // top edge outer
    DrawDashedHLine(scrn, bx+FOCUS_MARGIN_PIXELS, by+FOCUS_MARGIN_PIXELS+1,    bw-FOCUS_MARGIN_PIXELS*2); // top edge inner
    DrawDashedHLine(scrn, bx+FOCUS_MARGIN_PIXELS, by+bh-FOCUS_MARGIN_PIXELS-1, bw-FOCUS_MARGIN_PIXELS*2); // bottom edge inner
    DrawDashedHLine(scrn, bx+FOCUS_MARGIN_PIXELS, by+bh-FOCUS_MARGIN_PIXELS,   bw-FOCUS_MARGIN_PIXELS*2); // bottom edge outer

    DrawDashedVLine(scrn, bx+FOCUS_MARGIN_PIXELS,      by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // left edge outer
    DrawDashedVLine(scrn, bx+FOCUS_MARGIN_PIXELS+1,    by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // left edge inner
    DrawDashedVLine(scrn, bx+bw-FOCUS_MARGIN_PIXELS-1, by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // right edge inner
    DrawDashedVLine(scrn, bx+bw-FOCUS_MARGIN_PIXELS,   by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // right edge outer
    break;
  default:
    break;
  }
}

void DrawBorderToCanvas(GFXcanvas16 *canvas, struct windw_s *wnd, int brd_style) {
  int bx, by, bw, bh;
  
  bx = 0;
  by = 0;
  bw = wnd->ww;
  bh = wnd->wh;

  switch (brd_style) {
  case border_outset:
    // highlight - top and left sides, 2 pixels wide each
    canvas->drawFastHLine(bx,   by,   bw,   color_highlight); //  top edge - outer
    canvas->drawFastVLine(bx,   by,   bh,   color_highlight); // left edge - outer
    canvas->drawFastHLine(bx+1, by+1, bw-2, color_highlight); //  top edge - inner
    canvas->drawFastVLine(bx+1, by+1, bh-2, color_highlight); // left edge - innder

    // shadow - bottom and right sides, 2 pixels wide each
    canvas->drawFastHLine(bx+1,    by+bh-1, bw-1, color_shadow); // bottom edge - outer
    canvas->drawFastVLine(bx+bw-1, by,      bh,   color_shadow); //  right edge - outer
    canvas->drawFastHLine(bx+2,    by+bh-2, bw-3, color_shadow); // bottom edge - inner
    canvas->drawFastVLine(bx+bw-2, by+1,    bh-2, color_shadow); //  right edge - inner
    break;
  case border_ridge:
    canvas->drawFastHLine(bx+10,      by+10,      bw-20, color_shadow); //  top margin line
    canvas->drawFastVLine(bx+10,      by+10,      bh-20, color_shadow); // left margin line
    canvas->drawFastHLine(bx+10,      by+bh - 10, bw-20, color_highlight); // bottom margin line    
    canvas->drawFastVLine(bx+bw - 10, 10,         bh-20, color_highlight); //   left margin line
    break;
  case border_outset_narrow: // Draw caption bar
    canvas->drawFastVLine(bx + 20,      by+20, 20,      color_highlight);  //   left edge
    canvas->drawFastHLine(bx + 20,      by+20, bw - 40, color_highlight);  //    top edge
    canvas->drawFastVLine(bx + bw - 20, by+20, 21,      color_cap_shadow); //  right edge
    canvas->drawFastHLine(bx + 20,      by+40, bw - 40, color_cap_shadow); // bottom edge
    break;
  case border_inset:
    // shadow - top and left sides, 2 pixels wide each
    canvas->drawFastHLine(bx,   by,   bw-1, color_shadow); //  top edge - outer
    canvas->drawFastVLine(bx,   by,   bh-1, color_shadow); // left edge - outer
    canvas->drawFastHLine(bx+1, by+1, bw-3, color_shadow); //  top edge - inner
    canvas->drawFastVLine(bx+1, by+1, bh-3, color_shadow); // left edge - innder

    // highlight - bottom and right sides, 2 pixels wide each
    canvas->drawFastHLine(bx+1,      by + bh-1, bw-1, color_highlight); // bottom edge - outer
    canvas->drawFastVLine(bx + bw-1, by,        bh-1, color_highlight); //  right edge - outer
    canvas->drawFastHLine(bx+2,      by + bh-2, bw-3, color_highlight); // bottom edge - inner
    canvas->drawFastVLine(bx + bw-2, by+1,      bh-3, color_highlight); //  right edge - inner
    break;
  case border_focus:
    DrawDashedHLineToCanvas(canvas, bx+FOCUS_MARGIN_PIXELS, by+FOCUS_MARGIN_PIXELS,      bw-FOCUS_MARGIN_PIXELS*2); // top edge outer
    DrawDashedHLineToCanvas(canvas, bx+FOCUS_MARGIN_PIXELS, by+FOCUS_MARGIN_PIXELS+1,    bw-FOCUS_MARGIN_PIXELS*2); // top edge inner
    DrawDashedHLineToCanvas(canvas, bx+FOCUS_MARGIN_PIXELS, by+bh-FOCUS_MARGIN_PIXELS-1, bw-FOCUS_MARGIN_PIXELS*2); // bottom edge inner
    DrawDashedHLineToCanvas(canvas, bx+FOCUS_MARGIN_PIXELS, by+bh-FOCUS_MARGIN_PIXELS,   bw-FOCUS_MARGIN_PIXELS*2); // bottom edge outer

    DrawDashedVLineToCanvas(canvas, bx+FOCUS_MARGIN_PIXELS,      by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // left edge outer
    DrawDashedVLineToCanvas(canvas, bx+FOCUS_MARGIN_PIXELS+1,    by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // left edge inner
    DrawDashedVLineToCanvas(canvas, bx+bw-FOCUS_MARGIN_PIXELS-1, by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // right edge inner
    DrawDashedVLineToCanvas(canvas, bx+bw-FOCUS_MARGIN_PIXELS,   by+FOCUS_MARGIN_PIXELS, bh-FOCUS_MARGIN_PIXELS*2); // right edge outer
    break;
  default:
    break;
  }
}

// ---------------------- basic Window  --------------------

void DrawWindowToCanvas(struct windw_s *wnd, GFXcanvas16 *canvas) {
  // in buffer, origin is (0,0)
  // on screen, origin is (wx,wy)
  DrawBorderToCanvas (canvas, wnd, border_outset);

  // center
  canvas->fillRect(2, 2, wnd->ww-4, wnd->wh-4, color_window);

  //Draw border, if requested.
  if (wnd->border_style == border_ridge) {
    DrawBorderToCanvas (canvas, wnd, border_ridge);
  }
}

void SetupWindow(struct windw_s *wnd, int x, int y, int w, int h, int brd_style, int buf) {
  wnd->wx = x;
  wnd->wy = y;
  wnd->ww = w;
  wnd->wh = h;
  wnd->border_style = brd_style;
  wnd->buffered = buf;
  Serial.print("SetupWindow(x=");
  Serial.print(x);
  Serial.print(", y=");
  Serial.print(y);
  Serial.print(", w=");
  Serial.print(w);
  Serial.print(", h=");
  Serial.print(h);
  Serial.print(", brd_style=");
  Serial.print(brd_style);
  Serial.print(", buf=");
  Serial.print(buf);
  Serial.println(")");
  if (buf) {
    wnd->canvas = new GFXcanvas16(w, h);
    DrawWindowToCanvas (wnd, wnd->canvas);
  } else {
    wnd->canvas = NULL;
  }
}

void DrawWindow(struct windw_s *wnd) {
  //Draw basic 3-D window.

  if (wnd->buffered) {
    tft.drawRGBBitmap(wnd->wx,   wnd->wy, wnd->canvas->getBuffer(), wnd->canvas->width(), wnd->canvas->height());
  } else { // !wnd->buffered
    DrawBorder (&tft, wnd, border_outset);

    // center
    tft.fillRect(wnd->wx+2, wnd->wy+2, wnd->ww-3, wnd->wh-3, color_window);

    //Draw border, if requested.
    if (wnd->border_style == border_ridge) {
      DrawBorder (&tft, wnd, border_ridge);
    }
  }
}



// ---------------------- Captioned Window  --------------------

void SetupCapWindow(struct cap_windw_s *cwnd, int x, int y, int w, int h, int brd, int buf, char *cap) {
  SetupWindow(&cwnd->windw, x, y, w, h, brd, buf);
  if (cap != NULL) {
    strncpy (cwnd->label, cap, WINDW_MAX_CAP);
  } else { // cap == NULL
    cwnd->label[0] = '\x0';
  }
  cwnd->label[WINDW_MAX_CAP-1] = '\x0'; // ensure null term.
}

void DrawCapWindow(struct cap_windw_s *cwnd) {
  struct windw_s *wnd;
  wnd = &cwnd->windw;

  DrawWindow (wnd);

  // Draw caption bar
  DrawBorder (&tft, wnd, border_outset_narrow);  

  // center
  tft.fillRect(wnd->wx+21, wnd->wy+21, wnd->ww-41, 19, color_cap_bar);

  // label
  tft.setTextSize(2);
	int x = (wnd->wx + wnd->ww / 2) - (strlen(cwnd->label) * CHAR_WIDTH_PIXELS / 2);
  tft.setCursor(x, wnd->wy+22);
  tft.setTextColor(color_cap_label);
  tft.setTextWrap(true);
  tft.print(cwnd->label);
}

// ---------------------- Captioned Text Window --------------------
void SetupCapTWindow(struct cap_twindw_s *ctwnd, char *cap, char *ln1, char *ln2) {
  int str_w;
  int wndw_w = 230; // minumum width
  int wndw_x;
  int wndw_y = 164; // default y
  int wndw_h = 150; // default height

  if (cap != NULL) {
    str_w = strlen(cap) * CHAR_WIDTH_PIXELS + 60;
    if (wndw_w < str_w) {
      wndw_w = str_w;
    }
  }
  if (ln1 != NULL) {
    str_w = strlen(ln1) * CHAR_WIDTH_PIXELS + 60;
    if (wndw_w < str_w) {
      wndw_w = str_w;
    }
  }
  if (ln2 != NULL) {
    str_w = strlen(ln2) * CHAR_WIDTH_PIXELS + 60;
    if (wndw_w < str_w) {
      wndw_w = str_w;
    }
  }
  if (wndw_w > tft.width()-1) { // limit to screen bounds
    wndw_w = tft.width()-1;
  }
  wndw_x = (tft.width()/2) - wndw_w/2;

  if (wndw_h > tft.height()-1) {
    wndw_h = tft.height()-1;
  }
  if (wndw_y + wndw_h > tft.height()) {
    wndw_y = (tft.height()/2) - wndw_h/2;
  }

  if (ln1 != NULL) {
    strncpy (ctwnd->line1, ln1, WINDW_MAX_LINE);
  } else { // ln1 == NULL
    ctwnd->line1[0] = '\x0';
  }
  ctwnd->line1[WINDW_MAX_LINE-1] = '\x0'; // ensure null term.

  if (ln2 != NULL) {
    strncpy (ctwnd->line2, ln2, WINDW_MAX_LINE);
  } else { // ln2 == NULL
    ctwnd->line2[0] = '\x0';
  }
  ctwnd->line2[WINDW_MAX_LINE-1] = '\x0';
  
  SetupCapWindow(&ctwnd->cwindw, wndw_x, wndw_y, wndw_w, wndw_h, 0/*border*/, 1/*buffered*/, cap);
}

void DrawCapTWindow(struct cap_twindw_s *ctwnd) {
  struct cap_windw_s *cwnd;
  struct windw_s *wnd;
  cwnd = &ctwnd->cwindw;
  wnd = &cwnd->windw;

  DrawCapWindow (cwnd);
	// Position and draw window body text.
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.setTextColor(color_wndw_text);
	int x = (wnd->wx + wnd->ww / 2) - (strlen(ctwnd->line1) * CHAR_WIDTH_PIXELS) / 2;
	if (strlen(ctwnd->line2) == 0) {
    tft.setCursor(x, wnd->wy+68);
    tft.print(ctwnd->line1);
  } else { // strlen(line2) != 0
    tft.setCursor(x, wnd->wy+56);
    tft.print(ctwnd->line1);
	  x = (wnd->wx + wnd->ww / 2) - (strlen(ctwnd->line2) * CHAR_WIDTH_PIXELS) / 2;
    tft.setCursor(x, wnd->wy+71);
    tft.print(ctwnd->line2);
  }
}



// ---------------------- Button functions --------------------

void DrawButtonLabelToCanvas(struct button_s *btn) {
	int pos = -1;                  // position in 'btn->label_str' where '^' was found
	char tlabel[BUTTON_MAX_LABEL]; // copy of 'btn->label_str' with no '^' character
  GFXcanvas16 *lc;
  struct windw_s *wnd;
  wnd = &btn->windw;

  lc = btn->label_canvas;
  lc->fillScreen(color_window); // Clear button label canvas (not tft)

	// Find and remove the ^ character and
	// set the appropriate hot key.
	strncpy(tlabel, btn->label_str, BUTTON_MAX_LABEL);
  tlabel[BUTTON_MAX_LABEL-1] = '\x0';
	for (int i = 0; i < strlen(tlabel); ++i) {
		if (tlabel[i] == '^') {
      char func_letter;
      
			pos = i;
      func_letter = toupper(tlabel[i + 1]);
      if (func_letter >= 'A' && func_letter < 'Z') {
			  btn->hotkey = func_keys[func_letter - 'A'];
      }
			for (int j = i; j < strlen(tlabel); ++j) {
				tlabel[j] = tlabel[j + 1];
      }
		}
	}
	if      (strcmp(tlabel, "OK"    ) == 0) btn->altkey = OKALT;
	else if (strcmp(tlabel, "CANCEL") == 0) btn->altkey = CANCELALT;

	// Center and draw text on button.
  lc->setTextSize(2);
  lc->setTextWrap(true);
  lc->setTextColor(color_btn_text);
  int x = (wnd->ww / 2) - (strlen(tlabel) * CHAR_WIDTH_PIXELS / 2);
  lc->setCursor(x, 8);
  lc->print(tlabel);

	// Underline the hot-key character.
	if (pos >= 0) {
    lc->drawFastHLine(x + pos * CHAR_WIDTH_PIXELS, 18, CHAR_WIDTH_PIXELS-1, color_btn_text);
  }
}

void SetupButton(struct button_s *btn, int x, int y, const char *lbl) {
  int w;

  // make button wider to fit label
  w = strlen(lbl) * CHAR_WIDTH_PIXELS + 12;
  if (w < BUTTON_WIDTH_PIXELS) {
    w = BUTTON_WIDTH_PIXELS;
  }
  //SetupWindow(&btn->windw, x, y, w, BUTTON_HEIGHT_PIXELS/*h*/, 0/*brd*/, 1/*buf*/); // buffer not working on HX8357?
  SetupWindow(&btn->windw, x, y, w, BUTTON_HEIGHT_PIXELS/*h*/, 0/*brd*/, 0/*buf*/);

  if (lbl != NULL) {
    strncpy (btn->label_str, lbl, BUTTON_MAX_LABEL);
  } else { // lbl == NULL
    btn->label_str[0] = '\x0';
  }
  btn->label_str[BUTTON_MAX_LABEL-1] = '\x0'; // ensure null term.

  btn->altkey = 0;
  btn->hotkey = 0;

  btn->label_canvas = new GFXcanvas16(w-4, BUTTON_HEIGHT_PIXELS-4);
  DrawButtonLabelToCanvas (btn);
}

void DrawButton(struct button_s *btn, bool has_focus) {
  struct windw_s *wnd;
  wnd = &btn->windw;

  DrawWindow (wnd);

  if (wnd->buffered) {
    tft.drawRGBBitmap(wnd->wx+2,   wnd->wy+2, btn->label_canvas->getBuffer(),
                                              btn->label_canvas->width(),
                                              btn->label_canvas->height());
  } else { // !wnd->buffered
	  int pos = -1;                  // position in 'btn->label_str' where '^' was found
  	char tlabel[BUTTON_MAX_LABEL]; // copy of 'btn->label_str' with no '^' character

    // Find and remove the ^ character and
    // set the appropriate hot key.
    strncpy(tlabel, btn->label_str, BUTTON_MAX_LABEL);
    tlabel[BUTTON_MAX_LABEL-1] = '\x0';
    for (int i = 0; i < strlen(tlabel); ++i) {
      if (tlabel[i] == '^') {
        char func_letter;
        
        pos = i;
        func_letter = toupper(tlabel[i + 1]);
        if (func_letter >= 'A' && func_letter < 'Z') {
          btn->hotkey = func_keys[func_letter - 'A'];
        }
        for (int j = i; j < strlen(tlabel); ++j) {
          tlabel[j] = tlabel[j + 1];
        }
      }
    }
    if      (strcmp(tlabel, "OK"    ) == 0) btn->altkey = OKALT;
    else if (strcmp(tlabel, "CANCEL") == 0) btn->altkey = CANCELALT;

    // Center and draw text on button.
    tft.setTextSize(2);
    tft.setTextWrap(true);
    tft.setTextColor(color_btn_text);
    int x = (wnd->wx + wnd->ww / 2) - (strlen(tlabel) * CHAR_WIDTH_PIXELS / 2);
    tft.setCursor(x, wnd->wy+12);
    tft.print(tlabel);

    // Underline the hot-key character.
    if (pos >= 0) {
      tft.drawFastHLine(x + pos * CHAR_WIDTH_PIXELS, wnd->wy+20, CHAR_WIDTH_PIXELS-1, color_btn_text);
    }
  } // end else (!wnd->buffered)
  if (has_focus) {
    DrawBorder (&tft, wnd, border_focus);
  }
}

void ClickButton (struct button_s *btn) {
  struct windw_s *wnd;
  wnd = &btn->windw;

  // Shift the image on the button down and right
	// to simulate button movement.
  tft.drawRGBBitmap(wnd->wx+3,   wnd->wy+3, btn->label_canvas->getBuffer(),
                                            btn->label_canvas->width(),
                                            btn->label_canvas->height());

	// Draw the button’s borders so the
	// button appears to be pressed.
  DrawBorder (&tft, wnd, border_inset);

  // TODO: link to something the button knows.
#ifdef USE_MP3
  if (mp3_found) {

# ifdef ESP32_S3_TFT
    musicPlayer.playFullFile("/radio_gaga.mp3");
    // "Interrupt based playback currently does not work on ESP32 platforms."
    // Source: https://learn.adafruit.com/adafruit-music-maker-featherwing/library-reference
    // Music will start, but after it starts playing, then more input forces the processor to reboot.
# else
    musicPlayer.startPlayingFile("/radio_gaga.mp3");
# endif

  }
#endif
}



// ---------------------- Event to Control functions --------------------

void DrawCtrl(struct tab_ctrl_s *ctrl, bool has_focus) {
  switch (ctrl->ctrl_type) {
  case ctrl_button:
    DrawButton ((struct button_s *)ctrl->ctrl_obj, has_focus);
    break;
  case ctrl_edit:
    DrawEdit ((struct edit_s *)ctrl->ctrl_obj, has_focus);
    break;
  default: // other kind of control.
    break; //  do nothing.
  }
}

void OnKeyCtrl(struct tab_ctrl_s *ctrl, char kb_in) {
  switch (ctrl->ctrl_type) {
  case ctrl_button:
    break;
  case ctrl_edit:
    OnKeyEdit ((struct edit_s *)ctrl->ctrl_obj, kb_in);
    break;
  default: // other kind of control.
    break; //  do nothing.
  }
}

void UpdateCtrl(struct tab_ctrl_s *ctrl) {
  switch (ctrl->ctrl_type) {
  case ctrl_button:
    break;
  case ctrl_edit:
    UpdateEdit ((struct edit_s *)ctrl->ctrl_obj);
    break;
  default: // other kind of control.
    break; //  do nothing.
  }
}

struct windw_s *GetCtrlWindow(struct tab_ctrl_s *ctrl) {
  struct windw_s *ret = NULL;

  switch (ctrl->ctrl_type) {
  case ctrl_windw:
    ret = (struct windw_s *)ctrl->ctrl_obj;
    break;
  case ctrl_cap_windw:
    ret = &((struct cap_windw_s *)ctrl->ctrl_obj)->windw;
    break;
  case ctrl_cap_twindw:
    ret = &((struct cap_twindw_s *)ctrl->ctrl_obj)->cwindw.windw;
    break;
  case ctrl_button:
    ret = &((struct button_s *)ctrl->ctrl_obj)->windw;
    break;
  case ctrl_edit:
    ret = &((struct edit_s *)ctrl->ctrl_obj)->windw;
    break;
  default: // other kind of control.
    break; //  do nothing.
  }
  return ret;
}

void OnClickCtrl(struct tab_ctrl_s *ctrl) {
  if (!was_click) {
    haptic_click ();
  }
  was_click = true;

  switch (ctrl->ctrl_type) {
  case ctrl_button:
    ClickButton ((struct button_s *)ctrl->ctrl_obj);
    break;
  default: // other kind of control.
    break; //  do nothing.
  }
}



// ------------------- Edit control -------------------------

void SetupEdit (struct edit_s *edt, int x, int y, int text_max_chars, const char *text_default) {
  int w;

  if (text_max_chars > EDIT_MAX_TEXT-1) {
    text_max_chars = EDIT_MAX_TEXT-1;
  }
  edt->text_max_chars = text_max_chars;

  // make control wider to fit allowed chars (Alternative: horizontal scrolling)
  w = text_max_chars * CHAR_WIDTH_PIXELS + 15;
  if (w < EDIT_WIDTH_PIXELS) {
    w = EDIT_WIDTH_PIXELS;
  }
  //SetupWindow(&edt->windw, x, y, w, EDIT_HEIGHT_PIXELS/*h*/, 0/*brd*/, 1/*buf*/);
  SetupWindow(&edt->windw, x, y, w, EDIT_HEIGHT_PIXELS/*h*/, 0/*brd*/, 0/*buf*/); // no use for buffering this

  if (text_default != NULL) {
    strncpy (edt->text_str, text_default, EDIT_MAX_TEXT);
  } else { // text_default == NULL
    edt->text_str[0] = '\x0';
  }
  edt->text_str[EDIT_MAX_TEXT-1] = '\x0'; // ensure null term.

  edt->cursor_idx = strlen(edt->text_str);
  edt->cursor_blink_on = false;
  edt->cursor_blink_count = 0;
}

void DrawEdit (struct edit_s *edt, bool has_focus) {
  struct windw_s *wnd;
  wnd = &edt->windw;
  int tx, ty;

  tx = wnd->wx+9;
  ty = wnd->wy+9;

  DrawBorder(&tft, wnd, border_inset);

  tft.fillRect(wnd->wx+2, wnd->wy+2, wnd->ww-4, wnd->wh-4, color_edit_backgr);

  // Draw text in control.
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.setTextColor(color_edit_text);
  tft.setCursor(tx, wnd->wy+9);
  tft.print(edt->text_str);
 
  if (has_focus) {
    DrawBorder(&tft, wnd, border_focus);
  }
}

void DrawEditCursor (struct edit_s *edt) {
  struct windw_s *wnd;
  wnd = &edt->windw;
  int tx, ty;
  uint16_t draw_erase_color; // cursor color or background color

  tx = wnd->wx+9;
  ty = wnd->wy+9;

  if (edt->cursor_blink_on) {
    draw_erase_color = color_edit_cursor; // draw
  } else { // !cursor_blink_on
    draw_erase_color = color_edit_backgr; // erase
  }
  tft.drawFastVLine(tx + edt->cursor_idx*CHAR_WIDTH_PIXELS-2, ty-2, CHAR_HEIGHT_PIXELS+2, draw_erase_color);
  tft.drawFastVLine(tx + edt->cursor_idx*CHAR_WIDTH_PIXELS-1, ty-2, CHAR_HEIGHT_PIXELS+2, draw_erase_color);

  edt->cursor_blink_count = 0; // wait before blinking again
}

void UpdateEdit (struct edit_s *edt) {
  edt->cursor_blink_count++;
  if (edt->cursor_blink_count > CURSOR_BLINK_COUNT) {
    edt->cursor_blink_on = !edt->cursor_blink_on;
    DrawEditCursor (edt);
  }
}

void DebugPrintChars(char *str) {
  int idx;

  idx = 0;
  while (str[idx] != '\x0') {
    Serial.print(idx);
    Serial.print(": '");
    Serial.print(str[idx]);
    Serial.print("', ");
    idx++;
  }
  Serial.print(idx);
  Serial.println(": NULL");
}

void OnKeyEdit (struct edit_s *edt, char kb_in) {
  int text_len;

  text_len = strlen (edt->text_str);
  if (kb_in == KEY_BACKSPACE) {
    if (edt->cursor_idx > 0) {
      //edt->text_str[text_len-1] = '\x0';

      // Move all characters
      //  FROM range: cursor_idx   ... text_len+1
      //    TO range: cursor_idx-1 ... text_len
      int src_idx, dst_idx;
      dst_idx = edt->cursor_idx-1;
      while (dst_idx <= text_len) {
        src_idx = dst_idx+1;
        edt->text_str[dst_idx] = edt->text_str[src_idx];
        dst_idx++;
      }
      edt->cursor_idx--;
      DrawEdit (edt, true);
      edt->cursor_blink_on = true;
      DrawEditCursor (edt);
    }
  }

  // insert a character
  if (kb_in >= MIN_PRINTABLE && kb_in <= MAX_PRINTABLE) {
    if (text_len < edt->text_max_chars) { // yes, enough room for more
      // Move all characters
      //  FROM range: cursor_idx   ... text_len
      //    TO range: cursor_idx+1 ... text_len+1
      int src_idx, dst_idx;
      src_idx = text_len;
      while (src_idx >= edt->cursor_idx) {
        dst_idx = src_idx+1;
        edt->text_str[dst_idx] = edt->text_str[src_idx];
        src_idx--;
      }
      edt->text_str[edt->cursor_idx] = kb_in; // user's character
      edt->cursor_idx++;
      DrawEdit (edt, true);
      edt->cursor_blink_on = true;
      DrawEditCursor (edt);
    }
  }

  if (kb_in == KEY_LEFT) {
    if (edt->cursor_idx > 0) {
      edt->cursor_blink_on = false;
      DrawEditCursor (edt);
      
      edt->cursor_idx--;

      edt->cursor_blink_on = true;
      DrawEditCursor (edt);
    }
  }
  if (kb_in == KEY_RIGHT) {
    if (edt->cursor_idx < text_len) {
      edt->cursor_blink_on = false;
      DrawEditCursor (edt);

      edt->cursor_idx++;

      edt->cursor_blink_on = true;
      DrawEditCursor (edt);
    }
  }
}

// ------------------- Label control -------------------------

void SetupLabel (struct label_s *lbl, int x, int y, int text_max_chars, const char *text_default) {
  int w;

  if (text_max_chars > LABEL_MAX_TEXT-1) {
    text_max_chars = LABEL_MAX_TEXT-1;
  }
  lbl->text_max_chars = text_max_chars;

  // make control wider to fit allowed chars (Alternative: horizontal scrolling)
  w = text_max_chars * CHAR_WIDTH_PIXELS + 15;
  if (w < LABEL_WIDTH_PIXELS) {
    w = LABEL_WIDTH_PIXELS;
  }
  //SetupWindow(&lbl->windw, x, y, w, LABEL_HEIGHT_PIXELS/*h*/, 0/*brd*/, 1/*buf*/);
  SetupWindow(&lbl->windw, x, y, w, LABEL_HEIGHT_PIXELS/*h*/, 0/*brd*/, 0/*buf*/); // no use for buffering this

  if (text_default != NULL) {
    strncpy (lbl->text_str, text_default, LABEL_MAX_TEXT);
  } else { // text_default == NULL
    lbl->text_str[0] = '\x0';
  }
  lbl->text_str[LABEL_MAX_TEXT-1] = '\x0'; // ensure null term.
}

void DrawLabel (struct label_s *lbl, bool has_focus) {
  struct windw_s *wnd;
  wnd = &lbl->windw;
  int tx, ty;

  tx = wnd->wx+9;
  ty = wnd->wy+9;

  DrawBorder(&tft, wnd, border_inset);

  tft.fillRect(wnd->wx+2, wnd->wy+2, wnd->ww-4, wnd->wh-4, color_label_backgr);

  // Draw text in control.
  tft.setTextSize(2);
  tft.setTextWrap(true);
  tft.setTextColor(color_label_text);
  tft.setCursor(tx, wnd->wy+9);
  tft.print(lbl->text_str);
 
  //if (has_focus) {
  //  DrawBorder(&tft, wnd, border_focus);
  //}
}