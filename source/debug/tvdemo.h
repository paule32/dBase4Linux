#ifndef TVDEMO_H
#define TVDEMO_H

extern "C" {
#include <stdio.h>
}
#define Uses_string

#define Uses_TVCodePage
#define Uses_TMenuBar
#define Uses_TMenuItem
#define Uses_TSubMenu
#define Uses_TDialog
#define Uses_TStaticText
#define Uses_TColorDialog
#define Uses_TColorGroup
#define Uses_TColorItem
#define Uses_TPalette
#define Uses_TButton
#define Uses_TCalculator
#define Uses_TView
#define Uses_TFileDialog
#define Uses_TRect
#define Uses_TStatusLine
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TKeys
#define Uses_MsgBox
#define Uses_fpstream
#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TDeskTop
#define Uses_TRect
#define Uses_TScrollBar
#define Uses_TApplication
#define Uses_TWindow
#define Uses_TDeskTop
#define Uses_TScreen
#define Uses_IOS_BIN
#define Uses_THelpWindow
// Needed to remap the "system" menu character
#define Uses_TVCodePage
#define Uses_TStreamableClass

#include "../includes/tv.h"

#include "gadgets.h"
#include "fileview.h"
#include "ascii.h"
#include "mousedlg.h"

#include "tdebughelpwindow.h"

#include <iostream>

using namespace std;

#define HELP_FILENAME   "debug.hlp"

const int cmAboutCmd    = 106; // JASC, conflict with standard cmOpen
const int cmPuzzleCmd   = 101;
const int cmCalendarCmd = 102;
const int cmAsciiCmd    = 103;
const int cmCalcCmd     = 104;
const int cmOpen        = 105;
const int cmHelp        = 150;

const int cmAbout     = 106;
const int cmMyHelp    = 150;
const int cmMySave    = 102;
const int cmMyOpen    = 105;

#define cmOpenCmd     cmOpen

const int cmChDirCmd    = 106;
const int cmMouseCmd    = 108;
const int cmColorCmd    = 109;
const int cmSaveCmd     = 110;
const int cmRestoreCmd  = 111;
const int cmTestInputBox= 112;
const int cmTestPicture = 113;

const int
hcAbout = 1000,
hcExit  = 1001,
  hcNocontext            = 1,
  hcAsciiTable           = 6,
  hcCalculator           = 4,
  hcCalendar             = 5,
  hcCancelBtn            = 35,
  hcFCChDirDBox          = 37,
  hcFChangeDir           = 15,
  hcFDosShell            = 16,
  hcFExit                = 17,
  hcFOFileOpenDBox       = 31,
  hcFOFiles              = 33,
  hcFOName               = 32,
  hcFOOpenBtn            = 34,
  hcFOpen                = 14,
  hcFile                 = 13,
  hcOCColorsDBox         = 39,
  hcOColors              = 28,
  hcOMMouseDBox          = 38,
  hcOMouse               = 27,
  hcORestoreDesktop      = 30,
  hcOSaveDesktop         = 29,
  hcOpenBtn              = 36,
  hcOptions              = 26,
  hcPuzzle               = 3,
  hcSAbout               = 8,
  hcSAsciiTable          = 11,
  hcSCalculator          = 12,
  hcSCalendar            = 10,
  hcSPuzzle              = 9,
  hcSystem               = 7,
  hcViewer               = 2,
  hcWCascade             = 22,
  hcWClose               = 25,
  hcWNext                = 23,
  hcWPrevious            = 24,
  hcWSizeMove            = 19,
  hcWTile                = 21,
  hcWZoom                = 20,
  hcWindows              = 18;

class TStatusLine;
class TMenuBar;
struct TEvent;
class TPalette;
class THeapView;
class TClockView;
class fpstream;

class TVDemo : public TApplication
{
public:
    TVDemo(int argc, char **argv );
    TVDemo(void);
    void init_desk(void);

    static TStatusLine *initStatusLine( TRect r );
    static TMenuBar *initMenuBar( TRect r );

    static uchar systemMenuIcon [];
    static uchar osystemMenuIcon[];

    virtual void handleEvent(TEvent& event);
    virtual void getEvent(TEvent& event);
    virtual TPalette& getPalette() const;
    virtual void idle();              // Updates heap and clock views

    static TVCodePageCallBack oldCPCallBack;
    static void cpCallBack(ushort *map);

private:
    TDebugHelpWindow *hlpwindow;
    THeapView *heap;                  // Heap view
    TClockView *clock;                // Clock view

    void aboutDlgBox();               // "About" box
    void asciiTable();                // Ascii table
    void calculator();                // Calculator
    void openFile( char *fileSpec );  // File Viewer
    void colors();                    // Color control dialog box
    void mouse();                     // Mouse control dialog box
    void outOfMemory();               // For validView() function
};

#endif // TVDEMO_H

