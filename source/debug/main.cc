#include "tvdemo.h"

uchar TVDemo::systemMenuIcon [] = "~\360~";
uchar TVDemo::osystemMenuIcon[] = "~\360~";

bool helpActive = false;

TVCodePageCallBack TVDemo::oldCPCallBack = nullptr;

class THintStatusLine: public TStatusLine
{
public:
     THintStatusLine(TRect& r, TStatusDef& d)
          : TStatusLine(r, d) {}
     const char* hint(ushort);
};

const char* THintStatusLine::hint(ushort context)
{
    switch (context)
    {
        case hcNocontext:
        return "~F1~ Help  ~Alt+X~ Exit";

        case hcExit:
        return "Alt-X to Exit";

        case hcFOpen:
        return "Open a file";

        case hcAbout:
        return "View the About Box";
    }
    return "~F1~ Help  ~Alt+X~ Exit";
}

TVDemo::TVDemo()
    : TProgInit( &TVDemo::initStatusLine,
                 &TVDemo::initMenuBar,
                 &TVDemo::initDeskTop)
{
    hlpwindow = nullptr;
    init_desk();
}

TVDemo::TVDemo(int argc, char **argv)
    : TProgInit( &TVDemo::initStatusLine,
                 &TVDemo::initMenuBar,
                 &TVDemo::initDeskTop)
{
    init_desk();
}

void TVDemo::init_desk(void)
{
    TRect r = getExtent();                      // Create the clock view.
    r.a.x = r.b.x - 9;      r.b.y = r.a.y + 1;
    clock = new TClockView( r );
    insert(clock);

    r = getExtent();                            // Create the heap view.
    r.a.x = r.b.x - 13;     r.a.y = r.b.y - 1;
    heap = new THeapView( r );
    insert(heap);
}

TStatusLine * TVDemo::initStatusLine(TRect r)
{
    r.a.y = r.b.y - 1;

    return (new THintStatusLine( r,
        *new TStatusDef( 50, 0xffff) +
          *new TStatusItem( "~F1~ Help", kbF1, cmHelp ) +
          *new TStatusItem( "~Alt-X~ Exit", kbAltX, cmQuit ) +
          *new TStatusItem( 0, kbAltF3, cmClose ) +
          *new TStatusItem( 0, kbF10, cmMenu ) +
          *new TStatusItem( 0, kbF5, cmZoom ) +
          *new TStatusItem( 0, kbCtrlF5, cmResize ) +
        *new TStatusDef( 50, 0xffff ) +
          *new TStatusItem( "Howdy", kbF1, cmHelp )
          )
  );
}

TMenuBar *TVDemo::initMenuBar(TRect r)
{
    TSubMenu& sub1 =
      *new TSubMenu("~=~", kbAltSpace, hcNocontext ) +
        *new TMenuItem( "~A~bout...", cmAboutCmd, kbNoKey, hcSAbout ) +
         newLine() +
        *new TMenuItem( "~P~uzzle", cmPuzzleCmd, kbNoKey, hcSPuzzle ) +
        *new TMenuItem( "Ca~l~endar", cmCalendarCmd, kbNoKey, hcSCalendar ) +
        *new TMenuItem( "Ascii ~T~able", cmAsciiCmd, kbNoKey, hcSAsciiTable ) +
        *new TMenuItem( "~C~alculator", cmCalcCmd, kbNoKey, hcCalculator );

    TSubMenu& sub2 =
      *new TSubMenu( "~F~ile", 0, hcFile ) +
        *new TMenuItem( "~O~pen...", cmOpenCmd, kbF3, hcFOpen, "F3" ) +
        *new TMenuItem( "~C~hange Dir...", cmChDirCmd, kbNoKey, hcFChangeDir ) +
         newLine() +
        *new TMenuItem( "S~h~ell", cmCallShell, kbNoKey, hcFDosShell ) +
        *new TMenuItem( "E~x~it", cmQuit, kbAltX, hcFExit, "Alt-X" );

    r.b.y =  r.a.y + 1;
    return (new TMenuBar( r, sub1 + sub2 /*+ sub3 + sub4 */ ) );
}



// Called each time the code page changes. In this example we only have
// potential code page changes at start-up.
void TVDemo::cpCallBack(ushort *map)
{
    TVCodePage::RemapString(systemMenuIcon,osystemMenuIcon,map);
    //TCalendarView::upArrowChar   = TVCodePage::RemapChar(TCalendarView::oupArrowChar,map);
    //TCalendarView::downArrowChar = TVCodePage::RemapChar(TCalendarView::odownArrowChar,map);

    // If the chain was already used call it
    if (oldCPCallBack)
        oldCPCallBack(map);
}

ushort executeDialog( TDialog* pD, void* data)
{
    ushort c=cmCancel;

    if (TProgram::application->validView(pD))
        {
        if (data)
        pD->setData(data);
        c = TProgram::deskTop->execView(pD);
        if ((c != cmCancel) && (data))
            pD->getData(data);
        CLY_destroy(pD);
        }

    return c;
}

void TVDemo::aboutDlgBox()
{
    TDialog *aboutBox = new TDialog(TRect(0, 0, 42, 13), "About");

    aboutBox->insert(
      new TStaticText(TRect(9, 2, 33, 9),
        "\003dBase4Linux - Debug v1.0\n\n"
        "\003Demo Version\n\n\n"
        "\003Copyright (c) 2016\n\n\n"
        "\003non-profit\n\003KALLUP - Software"
        )
      );

    aboutBox->insert(
      new TButton(TRect(14, 10, 26, 12), " OK", cmOK, bfDefault)
      );

    aboutBox->options |= ofCentered;
    executeDialog(aboutBox);
}

void TVDemo::asciiTable()
{
    TAsciiChart *chart = (TAsciiChart *) validView(new TAsciiChart);

    if(chart != 0)
    {
        chart->helpCtx = hcAsciiTable;
        deskTop->insert(chart);
    }
}

void TVDemo::calculator()
{
    TCalculator *calc = (TCalculator *) validView(new TCalculator);

    if(calc != 0)
    {
        calc->helpCtx = hcCalculator;
        deskTop->insert(calc);
    }
}

void TVDemo::colors()
{
    TColorGroup &group1 =
        *new TColorGroup("Desktop") +
            *new TColorItem("Color",             1)+

        *new TColorGroup("Menus") +
            *new TColorItem("Normal",            2)+
            *new TColorItem("Disabled",          3)+
            *new TColorItem("Shortcut",          4)+
            *new TColorItem("Selected",          5)+
            *new TColorItem("Selected disabled", 6)+
            *new TColorItem("Shortcut selected", 7
        );

    TColorGroup &group2 =
        *new TColorGroup("Dialogs/Calc") +
            *new TColorItem("Frame/background",  33)+
            *new TColorItem("Frame icons",       34)+
            *new TColorItem("Scroll bar page",   35)+
            *new TColorItem("Scroll bar icons",  36)+
            *new TColorItem("Static text",       37)+

            *new TColorItem("Label normal",      38)+
            *new TColorItem("Label selected",    39)+
            *new TColorItem("Label shortcut",    40
        );

    TColorItem &item_coll1 =
        *new TColorItem("Button normal",     41)+
        *new TColorItem("Button default",    42)+
        *new TColorItem("Button selected",   43)+
        *new TColorItem("Button disabled",   44)+
        *new TColorItem("Button shortcut",   45)+
        *new TColorItem("Button shadow",     46)+
        *new TColorItem("Cluster normal",    47)+
        *new TColorItem("Cluster selected",  48)+
        *new TColorItem("Cluster shortcut",  49
        );

    TColorItem &item_coll2 =
        *new TColorItem("Input normal",      50)+
        *new TColorItem("Input selected",    51)+
        *new TColorItem("Input arrow",       52)+

        *new TColorItem("History button",    53)+
        *new TColorItem("History sides",     54)+
        *new TColorItem("History bar page",  55)+
        *new TColorItem("History bar icons", 56)+

        *new TColorItem("List normal",       57)+
        *new TColorItem("List focused",      58)+
        *new TColorItem("List selected",     59)+
        *new TColorItem("List divider",      60)+

        *new TColorItem("Information pane",  61
        );

     group2 = group2 + item_coll1 + item_coll2;

     TColorGroup &group3 =
         *new TColorGroup("Viewer") +
             *new TColorItem("Frame passive",      8)+
             *new TColorItem("Frame active",       9)+
             *new TColorItem("Frame icons",       10)+
             *new TColorItem("Scroll bar page",   11)+
             *new TColorItem("Scroll bar icons",  12)+
             *new TColorItem("Text",              13)+
         *new TColorGroup("Puzzle")+
             *new TColorItem("Frame passive",      8)+
             *new TColorItem("Frame active",       9)+
             *new TColorItem("Frame icons",       10)+
             *new TColorItem("Scroll bar page",   11)+
             *new TColorItem("Scroll bar icons",  12)+
             *new TColorItem("Normal text",       13)+
             *new TColorItem("Highlighted text",  14
         );


     TColorGroup &group4 =
         *new TColorGroup("Calendar") +
             *new TColorItem("Frame passive",     16)+
             *new TColorItem("Frame active",      17)+
             *new TColorItem("Frame icons",       18)+
             *new TColorItem("Scroll bar page",   19)+
             *new TColorItem("Scroll bar icons",  20)+
             *new TColorItem("Normal text",       21)+
             *new TColorItem("Current day",       22)+

         *new TColorGroup("Ascii table") +
             *new TColorItem("Frame passive",     24)+
             *new TColorItem("Frame active",      25)+
             *new TColorItem("Frame icons",       26)+
             *new TColorItem("Scroll bar page",   27)+
             *new TColorItem("Scroll bar icons",  28)+
             *new TColorItem("Text",              29
         );


    TColorGroup &group5 = group1 + group2 + group3 + group4;

    TPalette *temp_pal=new TPalette(getPalette());
    TColorDialog *c = new TColorDialog(temp_pal, &group5 );

    if( validView( c ) != 0 )
    {
        c->helpCtx = hcOCColorsDBox;  // set context help constant
        c->setData(&getPalette());
        if( deskTop->execView( c ) != cmCancel )
            {
            getPalette() = *(c->pal);
            setScreenMode(TScreen::screenMode);
            }
        CLY_destroy(c);
    }
    delete temp_pal;
}

static Boolean isTileable(TView *p, void * )
{
   if( (p->options & ofTileable) != 0)
       return True;
   else
       return False;
}


//
// Mouse Control Dialog Box function
//
void TVDemo::mouse()
{
    TMouseDialog *mouseCage = (TMouseDialog *) validView( new TMouseDialog() );

    if (mouseCage != 0)
        {
        mouseCage->helpCtx = hcOMMouseDBox;
        mouseCage->setData(&(TEventQueue::mouseReverse));
        if (deskTop->execView(mouseCage) != cmCancel)
            mouseCage->getData(&(TEventQueue::mouseReverse));
        }
    CLY_destroy(mouseCage);

}

//
// File Viewer function
//
void TVDemo::openFile( char *fileSpec )
{
    TFileDialog *d= (TFileDialog *)validView(
    new TFileDialog(fileSpec, "Open a File", "~N~ame", fdOpenButton, 100 ));

    if( d != 0 && deskTop->execView( d ) != cmCancel )
        {
        char fileName[PATH_MAX];
        d->getFileName( fileName );
        d->helpCtx = hcFOFileOpenDBox;
        TView *w= validView( new TFileWindow( fileName ) );
        if( w != 0 )
            deskTop->insert(w);
    }
    CLY_destroy(d);
}


//
// "Out of Memory" function ( called by validView() )
//
void TVDemo::outOfMemory()
{
    messageBox( "Not enough memory available to complete operation.",
      mfError | mfOKButton );
}

//
// getPalette() function ( returns application's palette )
//
#define cpAppColor \
       "\x71\x70\x78\x74\x20\x28\x24\x17\x1F\x1A\x31\x31\x1E\x71\x1F" \
    "\x37\x3F\x3A\x13\x13\x3E\x21\x3F\x70\x7F\x7A\x13\x13\x70\x7F\x7E" \
    "\x70\x7F\x7A\x13\x13\x70\x70\x7F\x7E\x20\x2B\x2F\x78\x2E\x70\x30" \
    "\x3F\x3E\x1F\x2F\x1A\x20\x72\x31\x31\x30\x2F\x3E\x31\x13\x38\x00" \
    "\x17\x1F\x1A\x71\x71\x1E\x17\x1F\x1E\x20\x2B\x2F\x78\x2E\x10\x30" \
    "\x3F\x3E\x70\x2F\x7A\x20\x12\x31\x31\x30\x2F\x3E\x31\x13\x38\x00" \
    "\x37\x3F\x3A\x13\x13\x3E\x30\x3F\x3E\x20\x2B\x2F\x78\x2E\x30\x70" \
    "\x7F\x7E\x1F\x2F\x1A\x20\x32\x31\x71\x70\x2F\x7E\x71\x13\x78\x00" \
    "\x37\x3F\x3A\x13\x13\x30\x3E\x1E"    // help colors

#define cpAppBlackWhite \
       "\x70\x70\x78\x7F\x07\x07\x0F\x07\x0F\x07\x70\x70\x07\x70\x0F" \
    "\x07\x0F\x07\x70\x70\x07\x70\x0F\x70\x7F\x7F\x70\x07\x70\x07\x0F" \
    "\x70\x7F\x7F\x70\x07\x70\x70\x7F\x7F\x07\x0F\x0F\x78\x0F\x78\x07" \
    "\x0F\x0F\x0F\x70\x0F\x07\x70\x70\x70\x07\x70\x0F\x07\x07\x08\x00" \
    "\x07\x0F\x0F\x07\x70\x07\x07\x0F\x0F\x70\x78\x7F\x08\x7F\x08\x70" \
    "\x7F\x7F\x7F\x0F\x70\x70\x07\x70\x70\x70\x07\x7F\x70\x07\x78\x00" \
    "\x70\x7F\x7F\x70\x07\x70\x70\x7F\x7F\x07\x0F\x0F\x78\x0F\x78\x07" \
    "\x0F\x0F\x0F\x70\x0F\x07\x70\x70\x70\x07\x70\x0F\x07\x07\x08\x00" \
    "\x07\x0F\x07\x70\x70\x07\x0F\x70"    // help colors

#define cpAppMonochrome \
       "\x70\x07\x07\x0F\x70\x70\x70\x07\x0F\x07\x70\x70\x07\x70\x00" \
    "\x07\x0F\x07\x70\x70\x07\x70\x00\x70\x70\x70\x07\x07\x70\x07\x00" \
    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07" \
    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x70\x00" \
    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07" \
    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x01\x00" \
    "\x70\x70\x70\x07\x07\x70\x70\x70\x0F\x07\x07\x0F\x70\x0F\x70\x07" \
    "\x0F\x0F\x07\x70\x07\x07\x70\x07\x07\x07\x70\x0F\x07\x07\x01\x00" \
    "\x07\x0F\x07\x70\x70\x07\x0F\x70"    // help colors

TPalette& TVDemo::getPalette() const
{
    static TPalette newcolor ( cpAppColor , sizeof( cpAppColor )-1 );
    static TPalette newblackwhite( cpAppBlackWhite , sizeof( cpAppBlackWhite )-1 );
    static TPalette newmonochrome( cpAppMonochrome , sizeof( cpAppMonochrome )-1 );
    static TPalette *palettes[] =
        {
        &newcolor,
        &newblackwhite,
        &newmonochrome
        };
    return *(palettes[appPalette]);

}

void TVDemo::handleEvent(TEvent& event)
{
    TApplication::handleEvent(event);
    switch(event.what)
    {
        case evMouseDown:
        {
            {
                if (helpActive) {
                    removeView(hlpwindow);
                    destroy(hlpwindow);
                }
            }
            deskTop->draw();
        }
        break;

        case evKeyDown:
        {
            if (event.keyDown.keyCode == kbF1)
            {
                if (helpActive)
                return;

                helpActive = true;
                hlpwindow  = new TDebugHelpWindow(TRect(5,5,40,17));

                insert(hlpwindow);
                clearEvent(event);
            }
            else if (event.keyDown.keyCode == kbEsc)
            {
                if (hlpwindow != nullptr) {
                    destroy(hlpwindow);
                    hlpwindow = nullptr;
                    helpActive = false;
                }
                clearEvent(event);
            }
        }
        break;

        case evCommand:             // handle COMMAND events
        {
            switch(event.message.command)
            {
            case cmAboutCmd:            //  About Dialog Box
//                if (hlpwindow != nullptr)
//                hlpwindow->hide();

                hlpwindow  = new TDebugHelpWindow(TRect(5,5,40,17));

                insert(hlpwindow);
                clearEvent(event);

                //aboutDlgBox();

                //hlpwindow->show();
                break;

            case cmAsciiCmd:            //  Ascii Table
                asciiTable();
                break;

            case cmCalcCmd:             //  Calculator
                calculator();
                break;

            case cmColorCmd:            //  Color control dialog box
                colors();
                break;

            default:
                    return;
            }
            clearEvent(event);      // Clear the event we handled
        }
    }
}

void TVDemo::getEvent(TEvent &event)
{
    TProgram::getEvent(event);
}

void TVDemo::idle()
{
    TProgram::idle();
    clock->update();
    heap->update();
    if (deskTop->firstThat(isTileable, 0) != 0 )
        {
        enableCommand(cmTile);
        enableCommand(cmCascade);
        }
    else
        {
        disableCommand(cmTile);
        disableCommand(cmCascade);
        }
}

TVDemo *debugApp;
int main(int argc, char **argv, char **envir)
{
    TDisplay::setArgv(argc,argv,envir);
    debugApp = new TVDemo(argc,argv);
    debugApp->deskTop->helpCtx = hcNocontext;

    TVDemo::oldCPCallBack=TVCodePage::SetCallBack(TVDemo::cpCallBack);

    const char *title = TScreen::getWindowTitle();
    TScreen::setWindowTitle("dBase4Linux debug v1.0 - Demo");

    if (title) {
        TScreen::setWindowTitle(title);
        delete[] title;
    }

    debugApp->run();
    debugApp->shutDown();

    TObject::CLY_destroy(debugApp);
    return 0;
}

