#include "GameView.hpp"
#include "Window.hpp"
#include <TranslationUtils.h>


GameView::GameView() : BView(BRect(0,0,WINDOW_WIDTH,WINDOW_HEIGHT),
	"GameView", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW | B_PULSE_NEEDED | B_FULL_UPDATE_ON_RESIZE)
{
	SetViewColor(0,85,0);
	//board=new Board(this);
	width=WINDOW_WIDTH;
	height=WINDOW_HEIGHT;
	points=0;
	moves=0;
	mouselock=false;
	selected=NULL;
	this->StartNewGame();
}

GameView::~GameView()
{
	
}

void
GameView::AllAttached()
{
	//GENERATE BOARD
	MakeFocus(true);
	BView::AttachedToWindow();
}

void
GameView::Draw(BRect rect)
{
	SetDrawingMode(B_OP_ALPHA);
	
	
	
	//DRAW SLOTS
	BPoint deckPoints[]={ BPoint(-80,4),BPoint(-80,116),BPoint(0,116),BPoint(0,4)};
	BPolygon* deck;
	
	for(int i=0;i<8;i++)
	{
		if(i==4)
		{
			for(int j=0;j<4;j++)
			{
				deckPoints[j].x+=80+4;
			}
		}
		for(int j=0;j<4;j++)
		{
			deckPoints[j].x+=80+4;
		}
		deck=new BPolygon(deckPoints,4);
		
		StrokePolygon(deck);
		
		if(board[i]!=NULL){
			//BBitmap* img=this->LoadBitmap(board[i]);
			DrawBitmap(board[i]->img,deck->Frame());
		}
	}
	//DRAW CARDS
	short row=0;
	short stack=0;
	for(short i=8;i<200;i++)
	{
		BBitmap* img=NULL;
		if(board[i]!=NULL)
		{
			//img=this->LoadBitmap(board[i]);
			img=board[i]->img;
		}
		BPoint pt(85*(stack+1),40*(row+4));
		DrawBitmapAsync(img,pt);
		/*if( board[i]!=NULL && board[i]->fSelected)
		{
			SetHighColor(255,0,0,127);
			FillRect(BRect(pt,BPoint(pt.x+80,pt.y+116)));
		}*/
		row++;
		if(row==26)
		{
			row=0;
			stack++;
		}
	}
	Sync();
	
	//DRAW POINTS
	
	BString pointsText = "";
	pointsText << points;
	
	BString movesText = "";
	movesText << moves;
	
	BFont bigFont;
	bigFont.SetFace(B_BOLD_FACE);
	bigFont.SetSize(18);
	
	BFont smallFont;
	smallFont.SetSize(12);
	
	SetHighColor(255,255,255);
	
	/*SetFont(&bigFont);
	DrawString(pointsText,BPoint((width+10 - bigFont.StringWidth(pointsText)) / 2, height-50));

	SetFont(&smallFont);
	DrawString("Points", BPoint((width+10  - smallFont.StringWidth("Points")) / 2, height-35));*/

	SetFont(&bigFont);
	DrawString(movesText, BPoint((width+10 - bigFont.StringWidth(movesText)) / 2, height-15));

	SetFont(&smallFont);
	DrawString("Moves", BPoint((width+10   - smallFont.StringWidth("Moves")) / 2, height));
}

void
GameView::Pulse()
{
	Invalidate();
}

void
GameView::MouseDown(BPoint point)
{
	if(mouselock)
		return;
	mouselock=true;
	card* card=NULL;
	short row=0;
	short stack=0;
	short saved=-1;
	bool special=false;
	for(short i=8;i<200;i++)
	{
		BPoint pt(85*(stack+1),40*(row+4));
		BRect rect(pt,BPoint(pt.x+80,pt.y+116));
		if(board[i]!=NULL)
		{
			if(rect.Contains(point))
			{
				//SELECT CARD
				saved=i;
			}
		}
		row++;
		if(row==26)
		{
			row=0;
			stack++;
		}
	}
	//CHECKING SPECIAL PLACES
	BPoint deckPoints[]={ BPoint(-80,4),BPoint(-80,116),BPoint(0,116),BPoint(0,4)};
	BPolygon* deck;
	
	for(short i=0;i<8;i++)
	{
		if(i==4)
		{
			for(int j=0;j<4;j++)
			{
				deckPoints[j].x+=80+4;
			}
		}
		for(int j=0;j<4;j++)
		{
			deckPoints[j].x+=80+4;
		}
		deck=new BPolygon(deckPoints,4);
		if(deck->Frame().Contains(point))
		{
			special=true;
			saved=i;
		}
	}
	if(special)
	{
		if(saved<4)
		{
			if(board[saved]==NULL)
				return;
			selected=board[saved];
			selected->oldNumber=saved;
			DragMessage(new BMessage(B_SIMPLE_DATA),new BBitmap(selected->img),B_OP_BLEND,BPoint(0,0));
		}
		return;
	}
	
	
	if(saved>=0 && this->CheckStack(saved))
	{
		//STACK OPTIONS
		short j=0;
		while(board[saved+j+1]!=NULL)
		{
			board[saved+j+1]->oldNumber=saved+j+1;
			board[saved+j]->fNextCard=board[saved+j+1];
			j++;
		}
		selected=board[saved];
		selected->oldNumber=saved;
		DragMessage(new BMessage(B_SIMPLE_DATA),new BBitmap(selected->img),B_OP_BLEND,BPoint(0,0));
	}

}

void
GameView::MouseMoved(BPoint point, uint32 transit, const BMessage* msg)
{
	/*if(mouselock && selected!=NULL)
	{
		//BBitmap* img=this->LoadBitmap(selected);
		DrawBitmapAsync(selected->img,point);
		Invalidate();
	}*/
}

void
GameView::MouseUp(BPoint point)
{
	mouselock=false;
	if(selected==NULL)
		return;
	card* card=NULL;
	short row=0;
	short stack=0;
	short saved=-1;
	bool special=false;
	for(short i=8;i<200;i++)
	{
		BPoint pt(85*(stack+1),40*(row+4));
		BRect rect(pt,BPoint(pt.x+80,pt.y+116));
		if(board[i]!=NULL)
		{
			if(rect.Contains(point))
			{
				//SELECT CARD
				saved=i;
			}
		}
		row++;
		if(row==26)
		{
			row=0;
			stack++;
		}
	}
	//CHECKING SPECIAL PLACES
	BPoint deckPoints[]={ BPoint(-80,4),BPoint(-80,116),BPoint(0,116),BPoint(0,4)};
	BPolygon* deck;
	
	for(short i=0;i<8;i++)
	{
		if(i==4)
		{
			for(int j=0;j<4;j++)
			{
				deckPoints[j].x+=80+4;
			}
		}
		for(int j=0;j<4;j++)
		{
			deckPoints[j].x+=80+4;
		}
		deck=new BPolygon(deckPoints,4);
		if(deck->Frame().Contains(point))
		{
			special=true;
			saved=i;
		}
	}
	if(special)
	{
		if(saved<4)
		{	
			if(selected->fNextCard==NULL && board[saved]==NULL)
			{
				board[saved]=selected;
				board[selected->oldNumber]=NULL;
				moves++;
			}
		}else{
			if(selected->fNextCard==NULL)
			{
				if(board[saved]==NULL && selected->fValue==1 )
				{
					goto putCardInStack;
				}
				if(board[saved]==NULL)
					return;
				if(board[saved]->fValue == selected->fValue-1 && selected->fColor == board[saved]->fColor)
				{
					putCardInStack:
					board[saved]=selected;
					board[selected->oldNumber]=NULL;
					moves++;
					
					if(board[4]->fValue==13 && board[5]->fValue==13 && board[6]->fValue==13 && board[7]->fValue==13)
					{
						//WIN
						BString winText="";
						winText << "WIN!!!\n\n";
						winText << "Congratulations, you have won SuperFreeCell!!\n\n";
						winText << "Moves: " << moves;
						BAlert* win=new BAlert("Win",winText,"OK");
						win->Go();
					}
				}
			}
		}
		selected=NULL;
		return;
	}
	//CHECKING VOID SPACES
	for(short j=0;j<8;j++)
	{
		if(board[j*26+8]==NULL)
		{
			BPoint pt(85*(j+1),40*(4));
			BRect rect(pt,BPoint(pt.x+80,pt.y+116));
			if(rect.Contains(point))
			{
				board[j*26+8]=selected;
				board[selected->oldNumber]=NULL;
				moves++;
				
				short k=0;
				while(selected->fNextCard!=NULL)
				{
					board[j*26+8+k+1]=selected->fNextCard;
					k++;
					struct card* temp_card=selected->fNextCard;
					selected->fNextCard=NULL;
					selected=temp_card;
					board[selected->oldNumber]=NULL;
				}
				selected=NULL;
				return;
			}
		}
	}
	if(saved>=0 && saved<200)
	{
		//SUPPORT FOR STACKS
		if(board[saved+1]==NULL && (board[saved]->fValue -1 == selected->fValue))
		{
			if(this->CheckSuite(selected->fColor,board[saved]->fColor))
			{
				short j=1;
				board[saved+1]=selected;
				board[selected->oldNumber]=NULL;
				moves++;
				
				while(selected->fNextCard!=NULL)
				{
					board[saved+j+1]=selected->fNextCard;
					j++;
					struct card* temp_card=selected->fNextCard;
					selected->fNextCard=NULL;
					selected=temp_card;
					board[selected->oldNumber]=NULL;
				}
			}
		}
	}
	selected=NULL;
}

void
GameView::StartNewGame()
{
	moves=0;
	for(short i=0;i<200;i++)
		board[i]=NULL;
	selected=NULL;
	srand(time(NULL));
	int totalCards[52]={0};
	this->Random(totalCards,52);
	
	board[8]=this->NumberToCard(totalCards[0]);
	board[9]=this->NumberToCard(totalCards[1]);
	board[10]=this->NumberToCard(totalCards[2]);
	board[11]=this->NumberToCard(totalCards[3]);
	board[12]=this->NumberToCard(totalCards[4]);
	board[13]=this->NumberToCard(totalCards[5]);
	board[14]=this->NumberToCard(totalCards[6]);
	
	board[34]=this->NumberToCard(totalCards[7]);
	board[35]=this->NumberToCard(totalCards[8]);
	board[36]=this->NumberToCard(totalCards[9]);
	board[37]=this->NumberToCard(totalCards[10]);
	board[38]=this->NumberToCard(totalCards[11]);
	board[39]=this->NumberToCard(totalCards[12]);
	board[40]=this->NumberToCard(totalCards[13]);
	
	board[60]=this->NumberToCard(totalCards[14]);
	board[61]=this->NumberToCard(totalCards[15]);
	board[62]=this->NumberToCard(totalCards[16]);
	board[63]=this->NumberToCard(totalCards[17]);
	board[64]=this->NumberToCard(totalCards[18]);
	board[65]=this->NumberToCard(totalCards[19]);
	board[66]=this->NumberToCard(totalCards[20]);
	
	board[86]=this->NumberToCard(totalCards[21]);
	board[87]=this->NumberToCard(totalCards[22]);
	board[88]=this->NumberToCard(totalCards[23]);
	board[89]=this->NumberToCard(totalCards[24]);
	board[90]=this->NumberToCard(totalCards[25]);
	board[91]=this->NumberToCard(totalCards[26]);
	board[92]=this->NumberToCard(totalCards[27]);
	
	board[112]=this->NumberToCard(totalCards[28]);
	board[113]=this->NumberToCard(totalCards[29]);
	board[114]=this->NumberToCard(totalCards[30]);
	board[115]=this->NumberToCard(totalCards[31]);
	board[116]=this->NumberToCard(totalCards[32]);
	board[117]=this->NumberToCard(totalCards[33]);
	
	board[138]=this->NumberToCard(totalCards[34]);
	board[139]=this->NumberToCard(totalCards[35]);
	board[140]=this->NumberToCard(totalCards[36]);
	board[141]=this->NumberToCard(totalCards[37]);
	board[142]=this->NumberToCard(totalCards[38]);
	board[143]=this->NumberToCard(totalCards[39]);
	
	board[164]=this->NumberToCard(totalCards[40]);
	board[165]=this->NumberToCard(totalCards[41]);
	board[166]=this->NumberToCard(totalCards[42]);
	board[167]=this->NumberToCard(totalCards[43]);
	board[168]=this->NumberToCard(totalCards[44]);
	board[169]=this->NumberToCard(totalCards[45]);
	
	board[190]=this->NumberToCard(totalCards[46]);
	board[191]=this->NumberToCard(totalCards[47]);
	board[192]=this->NumberToCard(totalCards[48]);
	board[193]=this->NumberToCard(totalCards[49]);
	board[194]=this->NumberToCard(totalCards[50]);
	board[195]=this->NumberToCard(totalCards[51]);
}

void
GameView::Resize(float wid,float he)
{
	ResizeTo(wid, he-20);
	width=wid-20;
	height=he-30;
}

void
GameView::Random(int v[], int size)
{
	int i, j, num, dupl;
	for (i = 0; i < size; i++){
 		num = 1 + rand() % size;
    	dupl = 0;
    	for (j = 0; j <= i; j++){
    		if (num == v[j]){
    			dupl = 1;
        		break;
        	}
        }
		if (dupl == 1)
			i--;
		else
			v[i] = num;
	}
}

card*
GameView::NumberToCard(int cd)
{
	card* cad=new card;
	cad->fValue=(cd % 13)+1;
	if(cd<14)
	{
		cad->fColor=1; //SPADES
	}else if(cd<27)
	{
		cad->fColor=3; //CLUBS
	}else if(cd<40)
	{
		cad->fColor=2; //HEARTS
	}else{
		cad->fColor=4; //DIAMONDS
	}
	cad->img=this->LoadBitmap(cad);
	cad->fNextCard=NULL;
	return cad;
}

BBitmap*
GameView::LoadBitmap(card* card)
{
	BString path="data/";
	path << card->fValue;
	path << "_";
	if(card->fColor == 1)
		path << "spade";
	if(card->fColor == 3)
		path << "club";
	if(card->fColor == 2)
		path << "heart";
	if(card->fColor == 4)
		path << "diamond";
	path << ".png";
	return BTranslationUtils::GetBitmap('rGFX',path);
}

bool
GameView::CheckStack(int card)
{
	if(board[card+1]==NULL)
	{
		return true;
	}
	if(board[card]->fValue - 1 == board[card+1]->fValue && this->CheckStack(card+1))
	{
		return true;
	}else{
		return false;
	}
}

bool
GameView::CheckSuite(int suit, int suit2)
{
	if(suit % 2 ==0 && suit2 % 2 == 1)
		return true;
	else if(suit % 2 == 1 && suit2 % 2 == 0)
		return true;
	else
		return false;
}
