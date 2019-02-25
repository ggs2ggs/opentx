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

#include "menu.h"

MenuWindow::MenuWindow(Menu * parent):
  Window(parent, {LCD_W / 2 - 100, LCD_H / 2 - 30 /* to avoid redraw the menus header */, 200, 0}, OPAQUE)
{
  setScrollbarColor(WARNING_COLOR);
  setFocus();
}

void MenuWindow::select(int index)
{
  selectedIndex = index;
  if (innerHeight > height()) {
    setScrollPositionY(lineHeight * index - 3 * lineHeight);
  }
  invalidate();
}

void MenuWindow::onKeyEvent(event_t event)
{
#if defined(DEBUG_WINDOWS)
  TRACE("%s received event 0x%X", getWindowDebugString().c_str(), event);
#endif

  if (event == EVT_ROTARY_RIGHT) {
    select((selectedIndex + 1) % lines.size());
  }
  else if (event == EVT_ROTARY_LEFT) {
    select(selectedIndex == 0 ? lines.size() - 1 : selectedIndex - 1);
  }
  else if (event == EVT_KEY_BREAK(KEY_ENTER)) {
    lines[selectedIndex].onPress();
    Window::onKeyEvent(event); // the window above will be closed on event
  }
  else if (event == EVT_KEY_BREAK(KEY_EXIT)) {
    Window::onKeyEvent(event);
  }
}

#if defined(TOUCH_HARDWARE)
bool MenuWindow::onTouchEnd(coord_t x, coord_t y)
{
  int index = y / lineHeight;
  lines[index].onPress();
  return false; // = close the menu (inverted so that click outside the menu closes it)
}
#endif

void MenuWindow::paint(BitmapBuffer * dc)
{
  int width = (innerHeight > height() ? 195 : 200);
  dc->clear(HEADER_BGCOLOR);
  for (unsigned i=0; i<lines.size(); i++) {
    dc->drawText(10, i * lineHeight + (lineHeight - 22) / 2, lines[i].text.data(), selectedIndex == (int)i ? WARNING_COLOR : MENU_TITLE_COLOR);
    if (i > 0) {
      dc->drawSolidHorizontalLine(0, i * lineHeight, width, CURVE_AXIS_COLOR);
    }
  }
}

void Menu::updatePosition()
{
  if (!toolbar) {
    // there is no navigation bar at the left, we may center the window on screen
    int count = min<int>(8, menuWindow.lines.size());
    coord_t h = count * MenuWindow::lineHeight;
    menuWindow.setTop((LCD_H - h) / 2);
    menuWindow.setHeight(h);
  }
  menuWindow.setInnerHeight(menuWindow.lines.size() * MenuWindow::lineHeight);
}

void Menu::addLine(const std::string & text, std::function<void()> onPress)
{
  menuWindow.addLine(text, std::move(onPress));
  updatePosition();
}

void Menu::removeLines()
{
  menuWindow.removeLines();
  updatePosition();
}

void Menu::onKeyEvent(event_t event)
{
  deleteLater();
}

#if defined(TOUCH_HARDWARE)
bool Menu::onTouchEnd(coord_t x, coord_t y)
{
  if (!Window::onTouchEnd(x, y)) {
    deleteLater();
  }
  return true;
}

bool Menu::onTouchSlide(coord_t x, coord_t y, coord_t startX, coord_t startY, coord_t slideX, coord_t slideY)
{
  Window::onTouchSlide(x, y, startX, startY, slideX, slideY);
  return true;
}
#endif
