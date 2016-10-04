#ifndef TDEBUGHELPWINDOW_H
#define TDEBUGHELPWINDOW_H

#define Uses_stdlib             // for exit(), rand()
#define Uses_iostream
#define Uses_fstream
#define Uses_stdio              // for puts() etc
#define Uses_string             // for strlen etc
#define Uses_ctype
#define Uses_getline

#define Uses_TEventQueue
#define Uses_TEvent
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TKeys
#define Uses_TRect
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TView
#define Uses_TWindow
#define Uses_THelpFile
#define Uses_TScroller
#define Uses_TVConfigFile
#define Uses_TDialog
#define Uses_TCheckBoxes
#define Uses_TLabel
#define Uses_TSItem
#define Uses_TRadioButtons
#define Uses_TInputLine
#define Uses_TValidator
#define Uses_TButton
#define Uses_MsgBox
#include "../includes/tv.h"

using namespace std;

class TInterior: public TScroller
{
public:
    TInterior(const TRect& bounds, TScrollBar *aHScrollBar, TScrollBar *aVScrollBar);
    virtual void draw();
};

class TDebugHelpViewer : public TScroller
{
public:
    TDebugHelpViewer(
            const TRect& bounds,
            TScrollBar *aHScrollBar,
            TScrollBar *aVScrollBar);
    ~TDebugHelpViewer();

    virtual void handleEvent(TEvent& event);

    void draw();
    void scrollDraw();

    Boolean valid( ushort command );
    Boolean isValid;
};

class TDebugHelpWindow : public TWindow
{
public:
    TDebugHelpWindow(TRect r);
    void makeInterior();
    TInterior *interior;
};

#endif // TDEBUGHELPWINDOW_H
