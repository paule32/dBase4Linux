#include "tvdemo.h"
#include "tdebughelpwindow.h"

const int maxLineLength = maxViewWidth+1;
const int maxLines      = 100;
char *Lines[maxLines];
int LineCount = 0;

extern bool helpActive;
extern class TVDemo debugApp;

static short winNumber = 0;

TDebugHelpWindow::TDebugHelpWindow(TRect r)
    : TWindowInit( &TDebugHelpWindow::initFrame )
    , TWindow(r, "Help", winNumber++)
{
    options |= ofTileable | ofFramed;
    state   |= sfCursorVis;

    TRect rec(getExtent());
    rec.grow(-1, -1);
    insert(new TDebugHelpViewer(rec
        , standardScrollBar(sbHorizontal | sbHandleKeyboard)
        , standardScrollBar(sbVertical   | sbHandleKeyboard)));
}

TDebugHelpViewer::~TDebugHelpViewer()
{
    helpActive = false;
}

Boolean TDebugHelpViewer::valid(ushort)  {
    return isValid;
}

TDebugHelpViewer::TDebugHelpViewer(
        const TRect& bounds,
        TScrollBar *aHScrollBar,
        TScrollBar *aVScrollBar)
    :   TScroller( bounds, aHScrollBar, aVScrollBar )
{
    growMode = gfGrowHiX | gfGrowHiY;
    isValid = True;
}

void TDebugHelpViewer::scrollDraw()
{
    TScroller::scrollDraw();
    draw();
}

void TDebugHelpViewer::draw()       // modified for scroller
{
    ushort color = getColor(0x0301);
    for( int i = 0; i < size.y; i++ )
        // for each line:
        {
        TDrawBuffer b;
        b.moveChar( 0, 'o', color, size.x );
        // fill line buffer with spaces
        int j = delta.y + i;       // delta is scroller offset
        if( j < LineCount && Lines[j] != 0 )
            {
            char s[maxLineLength];
            if( delta.x > (int)strlen(Lines[j] ) )
                s[0] = EOS;
            else
                {
                strncpy( s, Lines[j]+delta.x, size.x );
                s[size.x] = EOS;
                }
            b.moveCStr( 0, s, color );
            }
        writeLine( 0, i, size.x, 1, b);
        }
}


void TDebugHelpViewer::handleEvent(TEvent &event)
{
    TScroller::handleEvent(event);
}
