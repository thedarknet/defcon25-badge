#ifndef GUI_H
#define GUI_H

#include <stdio.h>
#include "display_device.h"
#include "fonts.h"


//running line
#define GUI_TickerSpeed 500
#define GUI_TickerEndDelay 3
class GUI_TickerData {
public:
	GUI_TickerData(const char * txt, uint8_t X, uint8_t Y, uint8_t W, uint8_t H);
	const char *text;
	uint8_t x, y, w, h, BorderSize, FontScalar;
	RGBColor bg, TextColor;
	uint32_t startTick;
};

class GUI_ListItemData {
public:
	GUI_ListItemData(uint8_t id1, const char *msg, bool scroll, uint16_t timeBetwenScrolls) :
			id(id1), text(msg), Scrollable(scroll), TimeBetweenScroll(timeBetwenScrolls), LastScrollTime(0), LastScrollPosition(
					0) {

	}
	GUI_ListItemData(uint8_t id, const char *msg) :
			Scrollable(0), TimeBetweenScroll(1000), LastScrollTime(0), LastScrollPosition(0) {
		this->id = id;
		text = msg;
	}
	GUI_ListItemData() :
			id(0), text(0), Scrollable(0), TimeBetweenScroll(1000), LastScrollTime(0), LastScrollPosition(0) {
	}
	void set(uint8_t n, const char *msg) {
		id = n;
		text = msg;
	}
	uint16_t id; /*!< Item's id */
	const char* text; /*!< Item's text*/
	uint16_t Scrollable :1;
	uint16_t TimeBetweenScroll :12;
	uint32_t LastScrollTime;
	uint8_t LastScrollPosition;
	const char *getScrollOffset();
	void setShouldScroll();
	bool shouldScroll();
	void resetScrollable() {
		Scrollable = 1;
		LastScrollTime = 0;
		LastScrollPosition = 0;
	}
};

class GUI_ListData {
public:
	GUI_ListData(const char *h, GUI_ListItemData *is, uint8_t x, uint8_t y, uint8_t w, uint8_t height, uint8_t si,
			uint8_t ic) {
		header = h;
		items = is;
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = height;
		selectedItem = si;
		ItemsCount = ic;
	}
	const char* header; /*!< Header*/
	GUI_ListItemData *items; /*!< Item's array*/
	uint16_t ItemsCount; /*!< Item's array*/
	uint8_t x, y, w, h;
	uint16_t selectedItem;
};

class GUI {
public:
	GUI(DisplayST7735 *display);
	bool init();
	void drawTicker(GUI_TickerData *dt);
	uint8_t drawList(GUI_ListData* list) const;

private:
	DisplayST7735 *Display;
};

#endif
