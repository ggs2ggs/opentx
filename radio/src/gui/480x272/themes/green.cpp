/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"

const ZoneOption OPTIONS_THEME_GREEN[] = {
  { STR_BACKGROUND_COLOR, ZoneOption::Color, OPTION_VALUE_UNSIGNED(LIGHTWHITE) },
  { STR_MAIN_COLOR, ZoneOption::Color, OPTION_VALUE_UNSIGNED(LIGHTGREEN) },
  { STR_TEXT_COLOR, ZoneOption::Color, OPTION_VALUE_UNSIGNED(BLACK) },
  { STR_BAR1_COLOR, ZoneOption::Color, OPTION_VALUE_UNSIGNED(ORANGE) },
  { STR_BAR2_COLOR, ZoneOption::Color, OPTION_VALUE_UNSIGNED(BRIGHTGREEN) },
  { NULL, ZoneOption::Bool }
};

class GreenTheme: public Theme
{
  public:
    GreenTheme():
      Theme("Green", OPTIONS_THEME_GREEN)
    {
    }

    void loadColors() const
    {
      lcdColorTable[TEXT_COLOR_INDEX] = BLACK;
      lcdColorTable[TEXT_BGCOLOR_INDEX] = LIGHTWHITE;
      lcdColorTable[TEXT_INVERTED_COLOR_INDEX] = LIGHTWHITE;
      lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = DARKGREEN;
      lcdColorTable[TEXT_STATUSBAR_COLOR_INDEX] = LIGHTWHITE;
      lcdColorTable[LINE_COLOR_INDEX] = GREY;
      lcdColorTable[SCROLLBOX_COLOR_INDEX] = DARKGREEN;
      lcdColorTable[MENU_TITLE_BGCOLOR_INDEX] = LIGHTWHITE;
      lcdColorTable[MENU_TITLE_COLOR_INDEX] = LIGHTWHITE;
      lcdColorTable[MENU_TITLE_DISABLE_COLOR_INDEX] = LIGHTWHITE;
      lcdColorTable[HEADER_COLOR_INDEX] = LIGHTGREY;
      lcdColorTable[ALARM_COLOR_INDEX] = RED;
      lcdColorTable[WARNING_COLOR_INDEX] = YELLOW;
      lcdColorTable[TEXT_DISABLE_COLOR_INDEX] = GREY;
      lcdColorTable[CURVE_AXIS_COLOR_INDEX] = LIGHTGREY;
      lcdColorTable[CURVE_COLOR_INDEX] = RED;
      lcdColorTable[CURVE_CURSOR_COLOR_INDEX] = DARKGREEN;
      lcdColorTable[TITLE_BGCOLOR_INDEX] = DARKGREEN;
      lcdColorTable[TRIM_BGCOLOR_INDEX] = LIGHTGREEN;
      lcdColorTable[TRIM_SHADOW_COLOR_INDEX] = BLACK;
      lcdColorTable[MAINVIEW_PANES_COLOR_INDEX] = LIGHTWHITE;
      lcdColorTable[MAINVIEW_GRAPHICS_COLOR_INDEX] = DARKGREEN;
      lcdColorTable[HEADER_BGCOLOR_INDEX] = DARKGREEN; // Top bar color
      lcdColorTable[HEADER_ICON_BGCOLOR_INDEX] = LIGHTGREEN; // background OPENTX icon
      lcdColorTable[HEADER_CURRENT_BGCOLOR_INDEX] = GREEN; // icons color
      lcdColorTable[OVERLAY_COLOR_INDEX] = LIGHTWHITE;
      lcdColorTable[BARGRAPH1_COLOR_INDEX] = ORANGE;
      lcdColorTable[BARGRAPH2_COLOR_INDEX] = BRIGHTGREEN;
      lcdColorTable[BARGRAPH_BGCOLOR_INDEX] = RGB(220, 220, 220);
    }

    void loadMenuIcon(uint8_t index, const char * filename) const
    {
      BitmapBuffer * mask = BitmapBuffer::loadMask(getThemePath(filename));
      if (mask) {
        menuIconNormal[index] = new BitmapBuffer(BMP_RGB565, mask->getWidth(), mask->getHeight());
        if (menuIconNormal[index]) {
          menuIconNormal[index]->clear(HEADER_BGCOLOR);
          menuIconNormal[index]->drawMask(0, 0, mask, HEADER_CURRENT_BGCOLOR);
        }
        menuIconSelected[index] = new BitmapBuffer(BMP_RGB565, mask->getWidth(), mask->getHeight());
        if (menuIconSelected[index]) {
          menuIconSelected[index]->clear(HEADER_BGCOLOR);
          menuIconSelected[index]->drawMask(0, 0, mask, MENU_TITLE_COLOR);
        }
        menuIconTitle[index] = new BitmapBuffer(BMP_RGB565, mask->getWidth(), mask->getHeight());
        if (menuIconTitle[index]) {
          menuIconTitle[index]->clear(HEADER_ICON_BGCOLOR);
          menuIconTitle[index]->drawMask(0, 0, mask, MENU_TITLE_COLOR);
        }
      }
    }

    void loadMenusIcons() const
    {
      if (isAsteriskDisplayed())
        loadMenuIcon(ICON_OPENTX, "mask_opentx_testmode.png");
      else
        loadMenuIcon(ICON_OPENTX, "mask_opentx.png");
      loadMenuIcon(ICON_RADIO, "mask_menu_radio.png");
      loadMenuIcon(ICON_RADIO_SETUP, "mask_radio_setup.png");
      loadMenuIcon(ICON_RADIO_SD_MANAGER, "mask_radio_sd_browser.png");
      loadMenuIcon(ICON_RADIO_TOOLS, "mask_radio_tools.png");
      loadMenuIcon(ICON_RADIO_SPECTRUM_ANALYSER, "/mask_radio_spectrum_analyser.png");
      loadMenuIcon(ICON_RADIO_GLOBAL_FUNCTIONS, "mask_radio_global_functions.png");
      loadMenuIcon(ICON_RADIO_TRAINER, "mask_radio_trainer.png");
      loadMenuIcon(ICON_RADIO_HARDWARE, "mask_radio_hardware.png");
      loadMenuIcon(ICON_RADIO_VERSION, "mask_radio_version.png");
      loadMenuIcon(ICON_MODEL, "mask_menu_model.png");
      loadMenuIcon(ICON_MODEL_SETUP, "mask_model_setup.png");
      loadMenuIcon(ICON_MODEL_HELI, "mask_model_heli.png");
      loadMenuIcon(ICON_MODEL_FLIGHT_MODES, "mask_model_flight_modes.png");
      loadMenuIcon(ICON_MODEL_INPUTS, "mask_model_inputs.png");
      loadMenuIcon(ICON_MODEL_MIXER, "mask_model_mixer.png");
      loadMenuIcon(ICON_MODEL_OUTPUTS, "mask_model_outputs.png");
      loadMenuIcon(ICON_MODEL_CURVES, "mask_model_curves.png");
      loadMenuIcon(ICON_MODEL_GVARS, "mask_model_gvars.png");
      loadMenuIcon(ICON_MODEL_LOGICAL_SWITCHES, "mask_model_logical_switches.png");
      loadMenuIcon(ICON_MODEL_SPECIAL_FUNCTIONS, "mask_model_special_functions.png");
      loadMenuIcon(ICON_MODEL_LUA_SCRIPTS, "mask_model_lua_scripts.png");
      loadMenuIcon(ICON_MODEL_TELEMETRY, "mask_model_telemetry.png");
      loadMenuIcon(ICON_STATS, "mask_menu_stats.png");
      loadMenuIcon(ICON_STATS_THROTTLE_GRAPH, "mask_stats_throttle_graph.png");
      loadMenuIcon(ICON_STATS_TIMERS, "mask_stats_timers.png");
      loadMenuIcon(ICON_STATS_ANALOGS, "mask_stats_analogs.png");
      loadMenuIcon(ICON_STATS_DEBUG, "mask_stats_debug.png");
      loadMenuIcon(ICON_THEME, "mask_menu_theme.png");
      loadMenuIcon(ICON_THEME_SETUP, "mask_theme_setup.png");
      loadMenuIcon(ICON_THEME_VIEW1, "mask_theme_view1.png");
      loadMenuIcon(ICON_THEME_VIEW2, "mask_theme_view2.png");
      loadMenuIcon(ICON_THEME_VIEW3, "mask_theme_view3.png");
      loadMenuIcon(ICON_THEME_VIEW4, "mask_theme_view4.png");
      loadMenuIcon(ICON_THEME_VIEW5, "mask_theme_view5.png");
      loadMenuIcon(ICON_THEME_ADD_VIEW, "mask_theme_add_view.png");
      loadMenuIcon(ICON_MONITOR, "mask_monitor.png");
      loadMenuIcon(ICON_MONITOR_CHANNELS1, "mask_monitor_channels1.png");
      loadMenuIcon(ICON_MONITOR_CHANNELS2, "mask_monitor_channels2.png");
      loadMenuIcon(ICON_MONITOR_CHANNELS3, "mask_monitor_channels3.png");
      loadMenuIcon(ICON_MONITOR_CHANNELS4, "mask_monitor_channels4.png");
      loadMenuIcon(ICON_MONITOR_LOGICAL_SWITCHES, "/mask_monitor_logsw.png");
    }

    void loadThemeBitmaps() const
    {
      // Calibration screen
      delete calibStick;
      calibStick = BitmapBuffer::load(getThemePath("stick_pointer.png"));

      delete calibStickBackground;
      calibStickBackground = BitmapBuffer::load(getThemePath("stick_background.png"));

      delete calibTrackpBackground;
      calibTrackpBackground = BitmapBuffer::load(getThemePath("trackp_background.png"));

      delete calibHorus;
#if defined(RADIO_T16)
      calibHorus = BitmapBuffer::load(getThemePath("t16.bmp"));
#elif defined(RADIO_T18)
      calibHorus = BitmapBuffer::load(getThemePath("t18.bmp"));
#elif defined(RADIO_TX16S)
      calibHorus = BitmapBuffer::load(getThemePath("tx16s.bmp"));
#elif defined(PCBX10)
      if(STICKS_PWM_ENABLED()) {
        calibHorus = BitmapBuffer::load(getThemePath("X10S.bmp"));
      }
      else {
        calibHorus = BitmapBuffer::load(getThemePath("X10.bmp"));
      }
#else
      calibHorus = BitmapBuffer::load(getThemePath("horus.bmp"));
#endif

      // Channels monitor screen
      delete chanMonLockedBitmap;
      chanMonLockedBitmap = BitmapBuffer::loadMaskOnBackground("mask_monitor_lockch.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete chanMonInvertedBitmap;
      chanMonInvertedBitmap = BitmapBuffer::loadMaskOnBackground("mask_monitor_inver.png", TEXT_COLOR, TEXT_BGCOLOR);

     // Model Selection screen
      delete modelselIconBitmap;
      modelselIconBitmap = BitmapBuffer::loadMaskOnBackground("modelsel/mask_iconback.png", TITLE_BGCOLOR, TEXT_BGCOLOR);
      if (modelselIconBitmap) {
        BitmapBuffer * bitmap = BitmapBuffer::load(getThemePath("modelsel/icon_default.png"));
        modelselIconBitmap->drawBitmap(20, 8, bitmap);
        delete bitmap;
      }

      delete modelselSdFreeBitmap;
      modelselSdFreeBitmap = BitmapBuffer::loadMaskOnBackground("modelsel/mask_sdfree.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete modelselModelQtyBitmap;
      modelselModelQtyBitmap = BitmapBuffer::loadMaskOnBackground("modelsel/mask_modelqty.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete modelselModelNameBitmap;
      modelselModelNameBitmap = BitmapBuffer::loadMaskOnBackground("modelsel/mask_modelname.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete modelselModelMoveBackground;
      modelselModelMoveBackground = BitmapBuffer::loadMask(getThemePath("modelsel/mask_moveback.png"));

      delete modelselModelMoveIcon;
      modelselModelMoveIcon = BitmapBuffer::loadMask(getThemePath("modelsel/mask_moveico.png"));

      delete modelselWizardBackground;
      modelselWizardBackground = BitmapBuffer::load(getThemePath("wizard/background.png"));


      // Mixer setup screen
      delete mixerSetupMixerBitmap;
      mixerSetupMixerBitmap = BitmapBuffer::loadMaskOnBackground("mask_sbar_mixer.png", MENU_TITLE_COLOR, HEADER_BGCOLOR);

      delete mixerSetupToBitmap;
      mixerSetupToBitmap = BitmapBuffer::loadMaskOnBackground("mask_sbar_to.png", TEXT_BGCOLOR, HEADER_BGCOLOR);

      delete mixerSetupOutputBitmap;
      mixerSetupOutputBitmap = BitmapBuffer::loadMaskOnBackground("mask_sbar_output.png", MENU_TITLE_COLOR, HEADER_BGCOLOR);

      delete mixerSetupAddBitmap;
      mixerSetupAddBitmap = BitmapBuffer::loadMaskOnBackground("mask_mplex_add.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupMultiBitmap;
      mixerSetupMultiBitmap = BitmapBuffer::loadMaskOnBackground("mask_mplex_multi.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupReplaceBitmap;
      mixerSetupReplaceBitmap = BitmapBuffer::loadMaskOnBackground("mask_mplex_replace.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupLabelBitmap;
      mixerSetupLabelBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_label.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupCurveBitmap;
      mixerSetupCurveBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_curve.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupSwitchBitmap;
      mixerSetupSwitchBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_switch.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupSlowBitmap;
      mixerSetupSlowBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_slow.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupDelayBitmap;
      mixerSetupDelayBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_delay.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupDelaySlowBitmap;
      mixerSetupDelaySlowBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_delayslow.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete mixerSetupFlightmodeBitmap;
      mixerSetupFlightmodeBitmap = BitmapBuffer::loadMaskOnBackground("mask_textline_fm.png", TEXT_COLOR, TEXT_BGCOLOR);

      delete topleftBitmap;
      topleftBitmap = BitmapBuffer::loadMaskOnBackground("topleft.png", HEADER_ICON_BGCOLOR, TITLE_BGCOLOR);
    }

    virtual void load() const
    {
      Theme::load();
      loadColors();
      loadMenusIcons();
      loadThemeBitmaps();
      if (!backgroundBitmap) {
        backgroundBitmap = BitmapBuffer::load(getThemePath("background.png"));
      }
      update();
    }

    virtual void update() const
    {
#define DARKER(x)     ((x * 70) / 100)

      uint32_t color = g_eeGeneral.themeData.options[1].unsignedValue;
      uint32_t bgColor = globalData.unexpectedShutdown ? WHITE : g_eeGeneral.themeData.options[0].unsignedValue;
      uint32_t textcolor = g_eeGeneral.themeData.options[2].unsignedValue;
      uint32_t barcolor = g_eeGeneral.themeData.options[3].unsignedValue;
      uint32_t bar2color = g_eeGeneral.themeData.options[4].unsignedValue;

      lcdColorTable[TEXT_COLOR_INDEX] = textcolor;
      lcdColorTable[TEXT_BGCOLOR_INDEX] = bgColor;
      lcdColorTable[TEXT_INVERTED_COLOR_INDEX] = bgColor;
      lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = color;
      lcdColorTable[TEXT_STATUSBAR_COLOR_INDEX] = bgColor;
      lcdColorTable[LINE_COLOR_INDEX] = GREY;
      lcdColorTable[SCROLLBOX_COLOR_INDEX] = color;
      lcdColorTable[MENU_TITLE_BGCOLOR_INDEX] = bgColor;
      lcdColorTable[MENU_TITLE_COLOR_INDEX] = bgColor;
      lcdColorTable[MENU_TITLE_DISABLE_COLOR_INDEX] = bgColor;
      lcdColorTable[HEADER_COLOR_INDEX] = LIGHTGREY;
      lcdColorTable[ALARM_COLOR_INDEX] = RED;
      lcdColorTable[WARNING_COLOR_INDEX] = YELLOW;
      lcdColorTable[TEXT_DISABLE_COLOR_INDEX] = GREY;
      lcdColorTable[CURVE_AXIS_COLOR_INDEX] = LIGHTGREY;
      lcdColorTable[CURVE_COLOR_INDEX] = RED;
      lcdColorTable[CURVE_CURSOR_COLOR_INDEX] = color;
      lcdColorTable[TITLE_BGCOLOR_INDEX] = color;
      lcdColorTable[TRIM_BGCOLOR_INDEX] = color;
      lcdColorTable[TRIM_SHADOW_COLOR_INDEX] = textcolor;
      lcdColorTable[MAINVIEW_PANES_COLOR_INDEX] = bgColor;
      lcdColorTable[MAINVIEW_GRAPHICS_COLOR_INDEX] = color;
      lcdColorTable[HEADER_BGCOLOR_INDEX] = color; // Top bar color
      lcdColorTable[HEADER_ICON_BGCOLOR_INDEX] = color; // background OPENTX icon
      lcdColorTable[HEADER_CURRENT_BGCOLOR_INDEX] = RGB(DARKER(GET_RED(color)), DARKER(GET_GREEN(color)), DARKER(GET_BLUE(color))); // icons color
      lcdColorTable[OVERLAY_COLOR_INDEX] = bgColor;
      lcdColorTable[BARGRAPH1_COLOR_INDEX] = barcolor;
      lcdColorTable[BARGRAPH2_COLOR_INDEX] = bar2color;
      lcdColorTable[BARGRAPH_BGCOLOR_INDEX] = RGB(DARKER(GET_RED(bgColor)), DARKER(GET_GREEN(bgColor)), DARKER(GET_BLUE(bgColor)));

      loadMenusIcons();
      loadThemeBitmaps();
      loadFontCache();
    }

    virtual void drawBackground() const
    {
      if (backgroundBitmap) {
        lcd->drawBitmap(0, 0, backgroundBitmap);
      }
      else {
        lcdSetColor(g_eeGeneral.themeData.options[0].unsignedValue);
        lcdDrawSolidFilledRect(0, 0, LCD_W, LCD_H, CUSTOM_COLOR);
      }
    }
    void drawTopbarBackground(uint8_t icon) const
    {
      if (topleftBitmap) {
        lcd->drawBitmap(0, 0, topleftBitmap);
        uint16_t width = topleftBitmap->getWidth();
        lcd->drawSolidFilledRect(width, 0, LCD_W-width, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
      }
      else {
        lcd->drawSolidFilledRect(0, 0, LCD_W, MENU_HEADER_HEIGHT, HEADER_BGCOLOR);
      }


      if (icon == ICON_OPENTX)
        lcd->drawBitmap(4, 10, menuIconTitle[ICON_OPENTX]);
      else
        lcd->drawBitmap(5, 7, menuIconTitle[icon]);

      drawTopbarDatetime();
    }

    virtual void drawMenuIcon(uint8_t index, uint8_t position, bool selected) const
    {
      lcd->drawBitmap(50+position*MENU_ICONS_SPACING, 7, selected ? menuIconSelected[index] : menuIconNormal[index]);
    }

  protected:
    static const BitmapBuffer * backgroundBitmap;
    static BitmapBuffer * topleftBitmap;
    static BitmapBuffer * menuIconNormal[MENUS_ICONS_COUNT];
    static BitmapBuffer * menuIconSelected[MENUS_ICONS_COUNT];
    static BitmapBuffer * menuIconTitle[MENUS_ICONS_COUNT];
};

const BitmapBuffer * GreenTheme::backgroundBitmap = NULL;
BitmapBuffer * GreenTheme::topleftBitmap = NULL;
BitmapBuffer * GreenTheme::menuIconNormal[MENUS_ICONS_COUNT] = { NULL };
BitmapBuffer * GreenTheme::menuIconSelected[MENUS_ICONS_COUNT] = { NULL };
BitmapBuffer * GreenTheme::menuIconTitle[MENUS_ICONS_COUNT] = { NULL };

GreenTheme greenTheme;
#if defined(DEFAULT_THEME_GREEN)
  Theme * theme = &greenTheme;
#endif