#ifndef _GUI_C_
#define _GUI_C_

#ifdef _NIX_

var pan_setwindow(PANEL *panel, var num, var x, var y, var dx, var dy, BMAP *bmap, var *px, var *py)
{

}

void bmap_blitpart (BMAP* bmDest, BMAP* bmSrc, VECTOR* vDestPos, VECTOR* vDestSize, VECTOR* vSrcPos, VECTOR* vSrcSize)
{

}

void pan_setdigits(PANEL *panel, var num, var x, var y, STRING *format, FONT *font, var factor, var *pv)
{

}

void pan_setbutton(PANEL*, var num, var type, var x, var y, BMAP* bmapOn, BMAP* bmapOff, BMAP* bmapOver, BMAP* bmapOverOff, void* functionClick, void* functionLeave, void* functionOver)
{

}

void pan_setslider(PANEL* panel, var num, var x, var y, BMAP* bmapBg, BMAP* bmapKnob, var min, var max, var* pv)
{

}

void pan_setcolor(PANEL* panel, var type, var num, COLOR* col)
{

}

#endif

#define PRAGMA_PATH "UI"

#define GWINDOW_MOVE		(1<<0)
#define GWINDOW_HIDE		(1<<1)
#define GWINDOW_CLOSE		(1<<2)
#define GWINDOW_COLLAPSED	(1<<3)

#define GWINDOW_DEFAULT (GWINDOW_MOVE | GWINDOW_HIDE | GWINDOW_CLOSE)

#define GMB_OK			(1<<0)
#define GMB_CANCEL		(1<<1)
#define GMB_YES			(1<<2)
#define GMB_NO			(1<<3)

#define GALIGN_LEFT		0
#define GALIGN_MIDDLE	1
#define GALIGN_CENTER	1
#define GALIGN_RIGHT	2

#define GFLAG_DONTWRITE_Y	(1<<15)

#define GDOCK_LEFT		(1<<0)
#define GDOCK_HCENTER	(1<<1)
#define GDOCK_RIGHT		(1<<2)
#define GDOCK_TOP		(1<<3)
#define GDOCK_VCENTER	(1<<4)
#define GDOCK_BOTTOM	(1<<5)

BMAP *gBmpWindowHeader = "ui_windowhead.png";
BMAP *gBmpWindowBackground = "ui_windowback.png";
BMAP *gBmpWindowKnobHide = "ui_hide.png";
BMAP *gBmpWindowKnobClose = "ui_close.png";
BMAP *gBmpButtonNormal = "ui_btn_normal.png";
BMAP *gBmpButtonOver = "ui_btn_over.png";
BMAP *gBmpButtonClicked = "ui_btn_click.png";
BMAP *gBmpSliderBackgroundHor = "ui_slider_bg_hor.png";
BMAP *gBmpSliderBackgroundVert = "ui_slider_bg_vert.png";
BMAP *gBmpSliderKnobHor = "ui_slider_knob_hor.png";
BMAP *gBmpSliderKnobVert = "ui_slider_knob_vert.png";
BMAP *gBmpListBackground = "ui_listback.png";
BMAP *gBmpSeparator = "ui_separator.png";
FONT *gFontWindow = "Arial#14b";

PANEL *gPanMainmenu = NULL;
PANEL *gPanMainmenuEntries[64];

var gDummyVar;

int gDefaultLayer = 16;
int gMenuLayer = 32;
int gMsgBoxLayer = 96;
int gWriteY = 1;

void *gEventClose = NULL;

BMAP *g_bmap_createhor(BMAP *source, var width)
{
	BMAP *bmp = bmap_createblack(width, 16, 8888);
	bmap_blitpart(bmp, source, vector(0, 0, 0), vector(16, 16, 0), vector(0, 0, 0), vector(16, 16, 0));
	bmap_blitpart(bmp, source, vector(16, 0, 0), vector(width-32, 16, 0), vector(16, 0, 0), vector(16, 16, 0));
	bmap_blitpart(bmp, source, vector(width-16, 0, 0), vector(16, 16, 0), vector(32, 0, 0), vector(16, 16, 0));
	return bmp;
}

BMAP *g_bmap_createvert(BMAP *source, var height)
{
	BMAP *bmp = bmap_createblack(16, height, 8888);
	bmap_blitpart(bmp, source, vector(0, 0, 0), vector(16, 16, 0), vector(0, 0, 0), vector(16, 16, 0));
	bmap_blitpart(bmp, source, vector(0, 16, 0), vector(16, height - 32, 0), vector(0, 16, 0), vector(16, 16, 0));
	bmap_blitpart(bmp, source, vector(0, height-16, 0), vector(16, 16, 0), vector(0, 32, 0), vector(16, 16, 0));
	return bmp;
}

void g_window_open(PANEL *pan)
{
	if(is(pan, SHOW)) return;
	set(pan, SHOW);
	PANEL *content = (PANEL*)pan->skill_x;
	if(pan->skill_y)
		set(content, SHOW);
}

void g_window_close(PANEL *pan)
{
	if(!is(pan, SHOW)) return;
	reset(pan, SHOW);
	PANEL *content = (PANEL*)pan->skill_x;
	pan->skill_y = is(content, SHOW);
	reset(content, SHOW);
}

void g_window_move()
{
	if(!mouse_panel) return;
	PANEL *pan = mouse_panel;
	VECTOR last;
	vec_set(&last, &mouse_pos);
	var ox = last.x - pan->pos_x;
	var oy = last.y - pan->pos_y;
	
	PANEL *panContent = (PANEL*)pan->skill_x;
	while(mouse_left)
	{
		last.x += mouse_pos.x - last.x;
		last.y += mouse_pos.y - last.y;
		pan->pos_x = clamp(last.x, 0, screen_size.x - 1) - ox;
		pan->pos_y = clamp(last.y, 0, screen_size.y - 1) - oy;
		panContent->pos_x = pan->pos_x;
		panContent->pos_y = pan->pos_y + 16;
		vec_set(&last, &mouse_pos);
		wait(1);
	}
}

void g_window_minimize()
{
	if(!mouse_panel) return;
	PANEL *pan = mouse_panel;
	pan = (PANEL*)pan->skill_x;
	toggle(pan, SHOW);
}

void g_window_close_event()
{
	if(!mouse_panel) return;
	g_window_close(mouse_panel);
}

PANEL *g_window_remove(PANEL *pan)
{
	PANEL *content = (PANEL*)pan->skill_x;
	ptr_remove((void*)content->skill_x);
	ptr_remove((void*)content->skill_y);
	ptr_remove(content);
	ptr_remove(pan);
}

void g_window_setpos(PANEL *pan, var x, var y)
{
	if(pan->skill_x == NULL) return;
	pan->pos_x = x;
	pan->pos_y = y;
	pan = (PANEL*)pan->skill_x;
	pan->pos_x = x;
	pan->pos_y = y + 16;
}

void g_window_expand(PANEL *pan)
{
	PANEL *content = (PANEL*)pan->skill_x;
	set(content, SHOW);
}

void g_window_collapse(PANEL *pan)
{
	PANEL *content = (PANEL*)pan->skill_x;
	reset(content, SHOW);
}

void g_window_dock(PANEL *pan, int dockstyle)
{
	PANEL *content = (PANEL*)pan->skill_x;
	
	if(dockstyle & GDOCK_LEFT)
		pan->pos_x = 0;
	if(dockstyle & GDOCK_HCENTER)
		pan->pos_x = 0.5 * (screen_size.x - pan->size_x);
	if(dockstyle & GDOCK_RIGHT)
		pan->pos_x = screen_size.x - pan->size_x;
	
	if(dockstyle & GDOCK_TOP)
		pan->pos_y = 16;
	if(dockstyle & GDOCK_VCENTER)
		pan->pos_y = 0.5 * (screen_size.y - content->size_y - 16);
	if(dockstyle & GDOCK_BOTTOM)
		pan->pos_y = screen_size.y - content->size_y - 16;
	
	content->pos_x = pan->pos_x;
	content->pos_y = pan->pos_y + 16;
}

PANEL *g_window_create(var x, var y, var width, STRING *title, long flags)
{
	y += 16;
	PANEL *pan = pan_create("", gDefaultLayer);
	
	BMAP *headBmp = g_bmap_createhor(gBmpWindowHeader, width);
	pan_setwindow(pan, 0, 0, 0, width, 16, headBmp, NULL, NULL);
	pan_setdigits(pan, 0, 4, 0, str_create(title), gFontWindow, 1, &gDummyVar);
	
	int knobPos = 16;
	if(flags & GWINDOW_CLOSE)
	{
		pan_setbutton(pan, 0, 0, width - knobPos, 0, gBmpWindowKnobClose, gBmpWindowKnobClose, gBmpWindowKnobClose, gBmpWindowKnobClose, g_window_close_event, NULL, NULL);
		knobPos += 16;
	}
	if(flags & GWINDOW_HIDE)
	{
		pan_setbutton(pan, 0, 0, width - knobPos, 0, gBmpWindowKnobHide, gBmpWindowKnobHide, gBmpWindowKnobHide, gBmpWindowKnobHide, g_window_minimize, NULL, NULL);
		knobPos += 16;
	}
	
	PANEL *panContent = pan_create("", gDefaultLayer);
	panContent->bmap = gBmpWindowBackground;
	panContent->size_x = width;
	panContent->pos_x = x;
	panContent->pos_y = y + 16;
	if(!(flags & GWINDOW_COLLAPSED))
		panContent->flags |= SHOW;
	pan->skill_x = panContent;
	
	// Store array of 50 pointers in skill_x
	panContent->skill_x = sys_malloc(50 * sizeof(void*));
	memset((void*)panContent->skill_x, 0, 50 * sizeof(void*));
	// Store array of 50 vars in skill_y
	panContent->skill_y = sys_malloc(50 * sizeof(var));
	memset((void*)panContent->skill_y, 0, 50 * sizeof(var));
	
	pan->pos_x = x;
	pan->pos_y = y;
	pan->size_x = width;
	pan->size_y = 16;
	if(flags & GWINDOW_MOVE)
		pan->event = g_window_move;
	set(pan, SHOW);
	return pan;
}

void g_window_addtext(PANEL *pan, STRING *text)
{
	if(pan->skill_x == NULL) return;
	pan = (PANEL*)pan->skill_x;
	pan_setdigits(pan, 0, 4, pan->size_y, str_create(text), gFontWindow, 1, &gDummyVar);
	if(gWriteY) pan->size_y += 16;
}

void g_window_adddigits(PANEL *pan, STRING *text, var *value)
{
	if(pan->skill_x == NULL) return;
	pan = (PANEL*)pan->skill_x;
	pan_setdigits(pan, 0, 4, pan->size_y, str_create(text), gFontWindow, 1, value);
	if(gWriteY) pan->size_y += 16;
}

void g_window_addstring(PANEL *pan, STRING *text)
{
	if(pan->skill_x == NULL) return;
	pan = (PANEL*)pan->skill_x;
	pan_setstring(pan, 0, 4, pan->size_y, gFontWindow, text);
	if(gWriteY) pan->size_y += 16;
}

void g_window_toggle_on(var button)
{
	if(!mouse_panel) return;
	if(mouse_panel->skill_x == NULL) return;
	var **array = (var**)mouse_panel->skill_x;
	var *arrayDigits = (var*)mouse_panel->skill_y;
	*(array[button]) = 1;
	
	pan_setcolor(mouse_panel, 1, arrayDigits[button], vector(0, 255, 0));
	pan_setcolor(mouse_panel, 1, arrayDigits[button]+1, vector(255, 255, 255));
}
void g_window_toggle_off(var button)
{
	if(!mouse_panel) return;
	if(mouse_panel->skill_x == NULL) return;
	var **array = (var**)mouse_panel->skill_x;
	var *arrayDigits = (var*)mouse_panel->skill_y;
	*(array[button]) = 0;
	
	pan_setcolor(mouse_panel, 1, arrayDigits[button]-1, vector(255, 255, 255));
	pan_setcolor(mouse_panel, 1, arrayDigits[button], vector(0, 0, 255));
}

void g_window_addtoggle(PANEL *pan, STRING *caption, var *value)
{
	if(pan->skill_x == NULL) return;
	pan = (PANEL*)pan->skill_x;
	if(pan->skill_x == NULL) return;
	var id, did;
	var **array = (var**)pan->skill_x;
	var *arrayDigits = (var*)pan->skill_y;
	pan_setdigits(pan, 0, 4, pan->size_y, str_create(caption), gFontWindow, 1, &gDummyVar);
	
	did = pan_setdigits(pan, 0, pan->size_x - 72 - str_width("On", gFontWindow) / 2 , pan->size_y, "On", gFontWindow, 1, &gDummyVar);
	if(*value != 0)
		pan_setcolor(pan, 1, did, vector(0, 255, 0));
	id = pan_setbutton(pan, 0, 0, pan->size_x - 96, pan->size_y, gBmpButtonClicked, gBmpButtonNormal, gBmpButtonOver, gBmpButtonOver, g_window_toggle_on, NULL, NULL);
	array[id] = value;
	arrayDigits[id] = did;
	
	did = pan_setdigits(pan, 0, pan->size_x - 24 - str_width("Off", gFontWindow) / 2, pan->size_y, "Off", gFontWindow, 1, &gDummyVar);
	if(*value == 0)
		pan_setcolor(pan, 1, did, vector(0, 0, 255));
	id = pan_setbutton(pan, 0, 0, pan->size_x - 48, pan->size_y, gBmpButtonClicked, gBmpButtonNormal, gBmpButtonOver, gBmpButtonOver, g_window_toggle_off, NULL, NULL);
	array[id] = value;
	arrayDigits[id] = did;
	
	if(gWriteY) pan->size_y += 16;
}

void g_window_addslider(PANEL *pan, var *value, var min, var max)
{
	if(pan->skill_x == NULL) return;
	pan = (PANEL*)pan->skill_x;
	
	BMAP *sliderBmp = g_bmap_createhor(gBmpSliderBackgroundHor, pan->size_x);
	pan_setslider(pan, 0, 0, pan->size_y, sliderBmp, gBmpSliderKnobHor, min, max, value);
	
	if(gWriteY) pan->size_y += 16;
}

void g_window_listbox_handler(PANEL *pan)
{
	while(handle(pan))
	{
		var h = pan->size_y - pan->red;
		if(h > 0)
		{
			pan->pos_y = -0.01 * h * pan->skill_x;
		}
		else
		{
			pan->pos_y = 0;
		}		
		wait(1);
	}
}

void g_listbox_add(PANEL *panel, STRING *item)
{
	int id = pan_setdigits(panel, 0, 4, panel->size_y, str_create(item), gFontWindow, 1, &gDummyVar);
	pan_setcolor(panel, 1, id, COLOR_WHITE);
	panel->size_y += 16;
}

int g_listbox_getselection(PANEL *panel)
{
	VECTOR pos;
	pan_getpos(panel, 3, 1, &pos);
	return integer(pos.y / 16);
}

void g_listbox_selectitem(var button)
{
	if(!mouse_panel) return;
	PANEL *pan = mouse_panel;
	
	PANEL **arrayPanels = (PANEL**)pan->skill_y;
	PANEL *panRender = arrayPanels[button];
	if(!panRender) return;
	VECTOR basePos;
	pan_getpos(pan, 3, button, &basePos);
	var dy = integer((mouse_pos.y - basePos.y - panRender->pos_y- pan->pos_y) / 16) * 16;
	if(dy < panRender->size_y)
		pan_setpos(panRender, 3, 1, vector(0, dy, 0));
}

PANEL *g_window_addlist(PANEL *pan, var height)
{
	if(pan->skill_x == NULL) return;
	pan = (PANEL*)pan->skill_x;
	height = maxv(height, 32);
	
	PANEL *panRender = pan_create("", 1);
	panRender->pos_x = 0;
	panRender->pos_y = 0;
	panRender->size_x = pan->size_x - 16;
	panRender->size_y = 0;
	panRender->red = height;
	panRender->green = 0;
	panRender->blue = 0;
	panRender->bmap = gBmpListBackground;
	panRender->skill_y = 0;
	panRender->target_map = bmap_createblack(panRender->size_x, height, 888);
	set(panRender, SHOW);
	
	BMAP *selectionKnob = bmap_createblack(panRender->size_x, 16, 8888);
	bmap_fill(selectionKnob, COLOR_WHITE, 10);
	pan_setbutton(panRender, 0, 0, 0, 0, selectionKnob, selectionKnob, selectionKnob, selectionKnob, NULL, NULL, NULL);
	
	BMAP *slider = g_bmap_createvert(gBmpSliderBackgroundVert, height);
	pan_setslider(pan, 0, pan->size_x - 16, pan->size_y, slider, gBmpSliderKnobVert, 0, 100, &panRender->skill_x);
	pan_setwindow(pan, 0, 0, pan->size_y, panRender->size_x, height, panRender->target_map, NULL, NULL);
	
	BMAP *listKnob = bmap_createblack(panRender->size_x, height, 8888);
	bmap_fill(listKnob, COLOR_RED, 00);
	int id = pan_setbutton(pan, 0, 0, 0, pan->size_y, listKnob, listKnob, listKnob, listKnob, g_listbox_selectitem, NULL, NULL);
	
	PANEL **arrayPanels = (PANEL**)pan->skill_y;
	arrayPanels[id] = panRender;
	
	if(gWriteY) pan->size_y += height;
	
	g_window_listbox_handler(panRender);
	
	return panRender;
}

void g_window_addbutton(PANEL *window, STRING *text, var width, int align, var offset, void *func)
{
	if(window == NULL) return;
	PANEL *pan = (PANEL*)window->skill_x;
	if(pan == NULL) return;
	if(text == NULL) return;
	
	BMAP *buttonNormal = g_bmap_createhor(gBmpButtonNormal, width);
	BMAP *buttonClicked = g_bmap_createhor(gBmpButtonClicked, width);
	BMAP *buttonOver = g_bmap_createhor(gBmpButtonOver, width);
	
	var pos = 0;
	if(align == GALIGN_LEFT)
		pos = 0;
	if(align == GALIGN_MIDDLE)
		pos = 0.5 * (pan->size_x - width);
	if(align == GALIGN_RIGHT)
		pos = pan->size_x - width;
	pos += offset;
	
	pan_setdigits(pan, 0, pos + 0.5 * (width - str_width(text, gFontWindow)), pan->size_y, str_create(text), gFontWindow, 1, &gDummyVar);
	pan_setbutton(pan, 0, 0, pos, pan->size_y, buttonClicked, buttonNormal, buttonOver, buttonOver, func, NULL, NULL);
	
	if(gWriteY) pan->size_y += 16;
}

void g_window_addpan(PANEL* window, PANEL *view, var height)
{
	if(window == NULL) return;
	PANEL *pan = (PANEL*)window->skill_x;
	if(pan == NULL) return;
	if(view == NULL) return;
	if(height <= 0) return;
	
	view->target_map = bmap_createblack(pan->size_x, height, 888);
	pan_setwindow(pan, 0, 0, pan->size_y, pan->size_x, height, view->target_map, NULL, NULL);
	
	if(gWriteY) pan->size_y += height;
}

void g_window_addview(PANEL* window, VIEW *view, var height)
{
	if(window == NULL) return;
	PANEL *pan = (PANEL*)window->skill_x;
	if(pan == NULL) return;
	if(view == NULL) return;
	if(height <= 0) return;
	
	view->bmap = bmap_createblack(pan->size_x, height, 888);
	pan_setwindow(pan, 0, 0, pan->size_y, pan->size_x, height, view->bmap, NULL, NULL);
	
	if(gWriteY) pan->size_y += height;
}

void g_window_addblank(PANEL *window, var height)
{
	if(window == NULL) return;
	PANEL *pan = (PANEL*)window->skill_x;
	if(pan == NULL) return;
	pan->size_y += height;
}

void g_window_addblank(PANEL *window)
{
	g_window_addblank(window, 16);
}

void g_window_addseparator(PANEL *window)
{
	if(window == NULL) return;
	PANEL *pan = (PANEL*)window->skill_x;
	if(pan == NULL) return;
	
	BMAP *separator = g_bmap_createhor(gBmpSeparator, pan->size_x);
	pan_setwindow(pan, 0, 0, pan->size_y, pan->size_x, 16, separator, NULL, NULL);
	
	pan->size_y += 16;
}

int gMsgBoxResult = 0;
int gMsgBoxOpened = 0;

void g_msgbox_set_ok()
{	
	gMsgBoxResult = GMB_OK;
}
void g_msgbox_set_cancel()
{	
	gMsgBoxResult = GMB_CANCEL;
}

void g_msgbox_set_yes()
{	
	gMsgBoxResult = GMB_YES;
}
void g_msgbox_set_no()
{	
	gMsgBoxResult = GMB_NO;
}

void g_msgbox_func(STRING *title, STRING *text, int style)
{
	PANEL *panBG = pan_create("", gMsgBoxLayer);
	panBG->pos_x = 0;
	panBG->pos_y = 0;
	panBG->size_x = screen_size.x;
	panBG->size_y = screen_size.y;
	panBG->bmap = bmap_createblack(screen_size.x, screen_size.y, 8888);
	bmap_fill(panBG->bmap, COLOR_BLACK, 25);
	set(panBG, SHOW);
	
	PANEL *panMsgBox = g_window_create(0.5 * screen_size.x - 128, 0.5 * screen_size.y - 96, 256, title, GWINDOW_CLOSE | GWINDOW_MOVE);
	PANEL *panContent = (PANEL*)panMsgBox->skill_x;
	panContent->size_y = 176;
	
	layer_sort(panMsgBox, gMsgBoxLayer + 1);
	layer_sort(panContent, gMsgBoxLayer + 1);
	set(panContent, SHOW);
	
	int btnCnt = 0;
	var btnY = panContent->size_y - 20;
	if(style == GMB_OK)
	{
		pan_setdigits(panContent, 0, 128 - 0.5 * str_width("Ok", gFontWindow), btnY, "Ok", gFontWindow, 1, &gDummyVar);
		pan_setbutton(panContent, 0, 0, 104, btnY, gBmpButtonClicked, gBmpButtonNormal, gBmpButtonOver,gBmpButtonOver, g_msgbox_set_ok, NULL, NULL);

		btnCnt = 1;
	}
	if(style == (GMB_OK | GMB_CANCEL))
	{
		pan_setdigits(panContent, 0, 104 - 0.5 * str_width("Ok", gFontWindow), btnY, "Ok", gFontWindow, 1, &gDummyVar);
		pan_setbutton(panContent, 0, 0, 78, btnY, gBmpButtonClicked, gBmpButtonNormal, gBmpButtonOver,gBmpButtonOver, g_msgbox_set_ok, NULL, NULL);
		
		pan_setdigits(panContent, 0, 154 - 0.5 * str_width("Cancel", gFontWindow), btnY, "Cancel", gFontWindow, 1, &gDummyVar);
		pan_setbutton(panContent, 0, 0, 130, btnY, gBmpButtonClicked, gBmpButtonNormal, gBmpButtonOver,gBmpButtonOver, g_msgbox_set_cancel, NULL, NULL);
		
		btnCnt = 2;
	}
	if(style == (GMB_YES | GMB_NO))
	{
		pan_setdigits(panContent, 0, 104 - 0.5 * str_width("Yes", gFontWindow), btnY, "Yes", gFontWindow, 1, &gDummyVar);
		pan_setbutton(panContent, 0, 0, 78, btnY, gBmpButtonClicked, gBmpButtonNormal, gBmpButtonOver,gBmpButtonOver, g_msgbox_set_yes, NULL, NULL);
		
		pan_setdigits(panContent, 0, 154 - 0.5 * str_width("No", gFontWindow), btnY, "No", gFontWindow, 1, &gDummyVar);
		pan_setbutton(panContent, 0, 0, 130, btnY, gBmpButtonClicked, gBmpButtonNormal, gBmpButtonOver,gBmpButtonOver, g_msgbox_set_no, NULL, NULL);
		
		btnCnt = 2;
	}
	if(style == (GMB_YES | GMB_NO | GMB_CANCEL))
	{
		pan_setdigits(panContent, 0, 76 - 0.5 * str_width("Yes", gFontWindow), btnY, "Yes", gFontWindow, 1, &gDummyVar);
		pan_setbutton(panContent, 0, 0, 52, btnY, gBmpButtonClicked, gBmpButtonNormal, gBmpButtonOver,gBmpButtonOver, g_msgbox_set_ok, NULL, NULL);
		
		pan_setdigits(panContent, 0, 128 - 0.5 * str_width("No", gFontWindow), btnY, "No", gFontWindow, 1, &gDummyVar);
		pan_setbutton(panContent, 0, 0, 104, btnY, gBmpButtonClicked, gBmpButtonNormal, gBmpButtonOver,gBmpButtonOver, g_msgbox_set_cancel, NULL, NULL);
		
		
		pan_setdigits(panContent, 0, 180 - 0.5 * str_width("Cancel", gFontWindow), btnY, "Cancel", gFontWindow, 1, &gDummyVar);
		pan_setbutton(panContent, 0, 0, 156, btnY, gBmpButtonClicked, gBmpButtonNormal, gBmpButtonOver,gBmpButtonOver, g_msgbox_set_cancel, NULL, NULL);
		
		btnCnt = 3;
	}
	
	if(btnCnt == 0)
	{
		error("g_msgbox: Invalid style passed!");
		return;
	}
	
	TEXT *txt = txt_create(1, gMsgBoxLayer + 2);
	txt->font = gFontWindow;
	(txt->pstring)[0] = text;
	set(txt, SHOW | WWRAP);
	gMsgBoxResult = 0;
	gMsgBoxOpened = 1;
	while(gMsgBoxResult == 0)
	{
		proc_mode = PROC_LATE;
		txt->pos_x = panContent->pos_x + 4;
		txt->pos_y = panContent->pos_y + 4;
		txt->size_x = panContent->size_x - 8;
		txt->size_y = panContent->size_y - 8;
		
		if(!is(panMsgBox, SHOW))
			gMsgBoxResult = GMB_CANCEL;
		wait(1);
	}
	gMsgBoxOpened = 0;	
	g_window_remove(panMsgBox);
	ptr_remove(panBG);
	ptr_remove(txt);
}

#define g_msgbox(title, text, style) if(!gMsgBoxOpened) { g_msgbox_func(title, text, style); while(gMsgBoxResult == 0) { wait(1); } } gMsgBoxOpened = gMsgBoxOpened

function g_event_close()
{
	if(gEventClose == NULL) return;
	function ev();
	ev = gEventClose;
	ev();
}

void g_window_open_menuentry()
{
	if(!mouse_panel) return;
	PANEL *pan = mouse_panel;
	pan = (PANEL*)pan->skill_x;
	int i;
	for(i = 0; i < 64; i++)
	{
		if(gPanMainmenuEntries[i] == NULL) continue;
		reset(gPanMainmenuEntries[i], SHOW);
	}
	set(pan, SHOW);
}

PANEL *g_menu_addentry(STRING *title)
{
	var width = str_width(title, gFontWindow) + 16;
	PANEL *pan = pan_create("", gMenuLayer + 1);
	pan->bmap =g_bmap_createhor(gBmpWindowHeader, width);
	
	pan_setdigits(pan, 0, 8, 0, str_create(title), gFontWindow, 1, &gDummyVar);
	
	PANEL *panContent = pan_create("", gMenuLayer + 2);
	panContent->bmap = gBmpWindowBackground;
	panContent->size_x = 192;
	panContent->pos_x = gPanMainmenu->skill_x;
	panContent->pos_y = 16;
	pan->skill_x = panContent;
	
	// Store array of 50 pointers in skill_x
	panContent->skill_x = sys_malloc(50 * sizeof(void*));
	memset((void*)panContent->skill_x, 0, 50 * sizeof(void*));
	// Store array of 50 vars in skill_y
	panContent->skill_y = sys_malloc(50 * sizeof(var));
	memset((void*)panContent->skill_y, 0, 50 * sizeof(var));
	
	pan->pos_x = gPanMainmenu->skill_x;
	pan->pos_y = 0;
	pan->size_x = width;
	pan->size_y = 16;
	pan->event = g_window_open_menuentry;
	set(pan, SHOW);
	
	gPanMainmenuEntries[gPanMainmenu->skill_y] = panContent;
	
	gPanMainmenu->skill_x += width;
	gPanMainmenu->skill_y += 1;
	
	
	return pan;
}

function g_init()
{
	gPanMainmenu = pan_create("", gMenuLayer);
	gPanMainmenu->pos_x = 0;
	gPanMainmenu->pos_y = 0;
	gPanMainmenu->size_x = screen_size.x;
	gPanMainmenu->size_y = 16;
	gPanMainmenu->bmap = g_bmap_createhor(gBmpWindowHeader, screen_size.x);	
	gPanMainmenu->flags = SHOW;
	pan_setbutton(gPanMainmenu, 0, 0, gPanMainmenu->size_x - 16, 0, gBmpWindowKnobClose, gBmpWindowKnobClose, gBmpWindowKnobClose, gBmpWindowKnobClose, g_event_close, NULL, NULL);
	
	var mouse_left_last = 0;
	while(1)
	{
		if(mouse_left != mouse_left_last)
		{
			if(mouse_left)
			{
				int i;
				for(i = 0; i < 64; i++)
				{
					if(gPanMainmenuEntries[i] == NULL) continue;
					if(gPanMainmenuEntries[i] != mouse_panel)
						reset(gPanMainmenuEntries[i], SHOW);
				}
			}
			mouse_left_last = mouse_left;
		}
		wait(1);
	}
}

#endif