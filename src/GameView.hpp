/*
 * Copyright 2014-2015 Adrián Arroyo Calle <adrian.arroyocalle@gmail.com>
 * All rights reserved. Distributed under the terms of the MIT license.
 */

#ifndef GAME_VIEW_HPP
#define GAME_VIEW_HPP

#include <InterfaceKit.h>

typedef struct card {
	short fValue;
	short fColor;
	bool fRevealed;
	short fEffect;
	bool fInPlay;
	bool fSelected;
	short oldNumber;
	bool stacked;
	card* fNextCard;
	card* fPrevCard;
	BBitmap* img;
} card;

class GameView : public BView{
public:
					GameView();
					~GameView();
	void			AllAttached();
	void			Draw(BRect rect);
	void			MouseDown(BPoint point);
	void			MouseMoved(BPoint, uint32 transit, const BMessage* msg);
	void			MouseUp(BPoint point);
	void			StartNewGame();
	void			Resize(float width, float height);
	card*			NumberToCard(int number);
	void			Random(int vec[],int num);
	BBitmap*		LoadBitmap(card* card);
	bool			CheckStack(int card);
	bool			CheckSuite(int suit, int suit2);
private:
	int32			points;
	int32			moves;
	float			height;
	float			width;
	card* 			board[200];
	bool			mouselock;
	card*			selected;
};

#endif
