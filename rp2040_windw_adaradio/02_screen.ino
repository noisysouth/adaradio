// ---------------------------- Define windows + controls. --------------------------
struct windw_s wnd_main;
//struct cap_windw_s cwnd_main;
//struct cap_twindw_s ctwnd_main;
//struct button_s btn_ok;
struct edit_s edit_text;
struct button_s btn_song1;
//struct button_s btn_song2;
//struct button_s btn_song3;
struct label_s lbl_fmfreq;
char fm_station_str[LABEL_MAX_TEXT];

struct ctrl_screen_s main_screen = {
  {
    { ctrl_edit,   &edit_text, },
    { ctrl_button, &btn_song1, },
//  { ctrl_button, &btn_song2, },
    { ctrl_type_none,  NULL, },
  },
  0, // focus_idx
};

struct ctrl_screen_s *cur_screen = &main_screen; // initial screen at start

// ---------------------------- Set up screen controls -----------------------------------
void setup_screen(void) {
  SetupWindow (&wnd_main, /*x0*/0, /*y0*/0, tft.width()-1, tft.height()-1, /*border*/1, /*buffered*/0);
  DrawWindow (&wnd_main);
  //SetupCapWindow (&cwnd_main, /*x0*/0, /*y0*/0, tft.width()-1, tft.height()-1, /*border*/1, /*buffered*/0, "MP3 Jukebox");
  //DrawCapWindow (&cwnd_main);
  //SetupCapTWindow (&ctwnd_main, "MP3 Jukebox", "radio_gaga.mp3", "top_gun_theme.mp3");
  //SetupCapTWindow (&ctwnd_main, "MP3 Jukebox", "", "");
  //DrawCapTWindow (&ctwnd_main);

  //SetupButton (&btn_ok, /*x*/(tft.width()/2)-BUTTON_WIDTH_PIXELS/2, /*y*/tft.height()-BUTTON_HEIGHT_PIXELS-6, "OK");
  //DrawButton (&btn_ok);
  SetupEdit (&edit_text, /*x*/20, /*y*/12, /*text_max_chars*/16, "My Radio");
  DrawEdit (&edit_text, /*has_focus*/1);
  
  //SetupButton (&btn_song1, /*x*/20, /*y*/tft.height()-BUTTON_HEIGHT_PIXELS*2-18, "radio_gaga.mp3");
  SetupButton (&btn_song1, /*x*/20, /*y*/12+EDIT_HEIGHT_PIXELS+6, "radio_gaga.mp3");
  DrawButton (&btn_song1, /*has_focus*/0);
  //SetupButton (&btn_song2, /*x*/20, /*y*/tft.height()-BUTTON_HEIGHT_PIXELS-12, "top_gun.mp3");
  //SetupButton (&btn_song2, /*x*/20, /*y*/12+EDIT_HEIGHT_PIXELS+6+BUTTON_HEIGHT_PIXELS+6, "top_gun.mp3");
  //DrawButton (&btn_song2, /*has_focus*/0);
  //SetupButton (&btn_song3, /*x*/10, /*y*/tft.height()-BUTTON_HEIGHT_PIXELS*2-12, "starships.mp3");
  //DrawButton (&btn_song3, /*has_focus*/1);
  SetupLabel (&lbl_fmfreq, /*x*/154, /*y*/12+EDIT_HEIGHT_PIXELS+6+BUTTON_HEIGHT_PIXELS+6, /*max_chars "102.1"*/5, /*text_default*/fm_station_str);
  DrawLabel (&lbl_fmfreq, /*has_focus*/0);
}
// ---------------------------- Set FM Station label -----------------------------------
void set_fm_station_label (char *new_fmt, ...) {
  va_list args;
  va_start (args, new_fmt);
  vsnprintf (fm_station_str, LABEL_MAX_TEXT, new_fmt, args);
  va_end (args);
}
// ----------------------- screen functions: change focus to new control ----------------------------

// Change focus to *next* control in screen structure.
//  If the last control had focus, wrap (change focus to the first control).
void ScreenTabNext(void) {
  struct ctrl_screen_s *scrn;
  int ctrl_idx;
  struct tab_ctrl_s *focus_old;
  struct tab_ctrl_s *focus_new;
  
  scrn = cur_screen;
  focus_old = &scrn->screen_ctrls[scrn->focus_idx];
  DrawCtrl (focus_old, /*has_focus*/false);

  ctrl_idx = 0;
  do {
    if (scrn->screen_ctrls[ctrl_idx].ctrl_type == ctrl_type_none) {
      scrn->focus_idx = 0; // go back to the first control
      break;
    }
    if (ctrl_idx == scrn->focus_idx + 1) {
      scrn->focus_idx = ctrl_idx; // we can go to the next one
      break;
    }
    ctrl_idx++;
  } while (1);

  focus_new = &scrn->screen_ctrls[scrn->focus_idx];
  DrawCtrl (focus_new, /*has_focus*/true);
}

// Change focus to *previous* control in screen structure.
//  If the first control had focus, wrap (change focus to the last control).
void ScreenTabPrev(void) {
  struct ctrl_screen_s *scrn;
  struct tab_ctrl_s *focus_old;
  struct tab_ctrl_s *focus_new;

  scrn = cur_screen;
  focus_old = &scrn->screen_ctrls[scrn->focus_idx];
  DrawCtrl (focus_old, /*has_focus*/false);

  if (scrn->focus_idx > 0) {
    scrn->focus_idx--;
  } else { // scrn->focus_idx <= 0. We are at the first control in the screen, going backwards.
    int ctrl_idx;

    ctrl_idx = 0;
    while (scrn->screen_ctrls[ctrl_idx].ctrl_type != ctrl_type_none) {
      ctrl_idx++;
    };
    scrn->focus_idx = ctrl_idx-1; // go to the last control
  }

  focus_new = &scrn->screen_ctrls[scrn->focus_idx];
  DrawCtrl (focus_new, /*has_focus*/true);
}

// On touching the screen:
// 1. Change focus to control at that location.
// 2. Click that control.
void ScreenTouch(int touch_x, int touch_y) {
  struct ctrl_screen_s *scrn;
  int old_idx;
  int ctrl_idx;
  struct tab_ctrl_s *chk_ctrl;
  struct windw_s *chk_windw;
  struct tab_ctrl_s *focus_old;
  struct tab_ctrl_s *focus_new = NULL;

  scrn = cur_screen;
  old_idx = scrn->focus_idx;
  ctrl_idx = 0;
  do {
    if (scrn->screen_ctrls[ctrl_idx].ctrl_type == ctrl_type_none) {
      return; // got to the end of the list, found no control being touched
    }

    chk_ctrl = &scrn->screen_ctrls[ctrl_idx];
    chk_windw = GetCtrlWindow(chk_ctrl); // Get X and Y of control
    if (chk_windw != NULL) {
      if (touch_x >= chk_windw->wx &&
          touch_x <= chk_windw->wx+chk_windw->ww &&
          touch_y >= chk_windw->wy &&
          touch_y <= chk_windw->wy+chk_windw->wh) {
        // touch is within bounds of window
        scrn->focus_idx = ctrl_idx;
        do_click  = true;
        //was_click = true;
        break;
      }
    }
    ctrl_idx++;
  } while (1);

  if (scrn->focus_idx != old_idx) {
    focus_old = &scrn->screen_ctrls[old_idx];
    DrawCtrl (focus_old, /*has_focus*/false);

    focus_new = &scrn->screen_ctrls[scrn->focus_idx];
    DrawCtrl (focus_new, /*has_focus*/true);
  }
}

// ------------------------- Send input to control that has focus --------------------------
void loop_screen(char kb_in, bool do_unclick) {
  struct tab_ctrl_s *focus_ctrl;

  focus_ctrl = &cur_screen->screen_ctrls[cur_screen->focus_idx];

  if (do_click) {
    OnClickCtrl (focus_ctrl);
  }
  if (do_unclick) {
    haptic_click ();
    DrawCtrl (focus_ctrl, /*has_focus*/true);
    was_click = false;
  }

  UpdateCtrl (focus_ctrl); // e.g. for cursor blink, etc.
  if (kb_in != 0) {
    OnKeyCtrl (focus_ctrl, kb_in);
  }
}
