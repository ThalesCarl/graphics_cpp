/////////////////////////////////////////////////////////////////////////////
// Name:        minesweaper.cpp
// Purpose:     Minimal wxWidgets sample used to create a minesweaper game
// Author:      Julian Smart
// Modified by: Thales Carl
// Modified:     10/01/21
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES 
     #include "./sample.xpm"
#endif


// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() wxOVERRIDE;
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);
    ~MyFrame();

    int nFieldWidth = 10;
    int nFieldHeight = 10;
    int numberOfMines = 20;
    wxButton **btn;

    void GameOver();
    void FirstClick(int buttonPostionX,int buttonPostionY);
    bool RegularClick(int buttonPostionX,int buttonPostionY);
    int  GetNumberOfNgbBombs(int x, int y);

    // event handlers (these functions should _not_ be virtual)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnButtonClicked(wxCommandEvent& event);
    void OnMarkField(wxMouseEvent& event);

private:
    int minesLeft = numberOfMines;

    int  *mineField = nullptr;
    bool *isMarked  = nullptr;
    bool firstClick = true;
    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Quit = wxID_EXIT,

    // it is important for the id corresponding to the "About" command to have
    // this standard value as otherwise it won't be handled properly under Mac
    // (where it is special and put into the "Apple" menu)
    Minimal_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

// the event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

// Create a new application object: this macro will allow wxWidgets to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also implements the accessor function
// wxGetApp() which will return the reference of the right type (i.e. MyApp and
// not wxApp)
wxIMPLEMENT_APP(MyApp);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

// 'Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    // call the base class initialization method, currently it only parses a
    // few common command-line options but it could be do more in the future
    if ( !wxApp::OnInit() )
        return false;

    // create the main application window
    MyFrame *frame = new MyFrame("Mine Sweeper");

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
MyFrame::MyFrame(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title)
{
    // set the frame icon
    SetIcon(wxICON(sample));

    btn = new wxButton *[nFieldWidth*nFieldHeight];
    wxGridSizer *grid = new wxGridSizer(nFieldWidth,nFieldHeight,0,0);

    wxFont font(24,wxFONTFAMILY_DEFAULT,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false);

    mineField = new int[nFieldWidth*nFieldHeight];
    isMarked = new bool[nFieldWidth*nFieldHeight];

    for (int i = 0; i < nFieldWidth; i++)
    {
        for (int j = 0; j < nFieldHeight; j++)
        {
            btn[j*nFieldWidth+i] = new wxButton(this,10000 + (j*nFieldWidth+i));
            btn[j*nFieldWidth+i]->SetFont(font);
            grid -> Add(btn[j*nFieldWidth+i],1,wxEXPAND | wxALL);

            btn[j*nFieldWidth+i]->Bind(wxEVT_COMMAND_BUTTON_CLICKED,&MyFrame::OnButtonClicked,this);
            btn[j*nFieldWidth+i]->Connect(wxEVT_RIGHT_DOWN, wxMouseEventHandler(MyFrame::OnMarkField), NULL, this);
            mineField[j*nFieldWidth+i] = 0;
        }
    }
    this->SetSizer(grid);
    grid->Layout();
    


#if wxUSE_MENUBAR
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#else // !wxUSE_MENUBAR
    // If menus are not available add a button to access the about box
    wxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* aboutBtn = new wxButton(this, wxID_ANY, "About...");
    aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);
    sizer->Add(aboutBtn, wxSizerFlags().Center());
    SetSizer(sizer);
#endif // wxUSE_MENUBAR/!wxUSE_MENUBAR

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText("Welcome");
#endif // wxUSE_STATUSBAR
}

MyFrame::~MyFrame()
{
    delete[]btn;
}


// event handlers

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("(c) 2021 ThalesCarl");
}

void MyFrame::OnButtonClicked(wxCommandEvent& event)
{
    //Which button was clicked?
    int x = (event.GetId()-10000)%nFieldWidth;
    int y = (event.GetId()-10000)/nFieldHeight;

    if(firstClick)
        FirstClick(x,y);

    bool safe = RegularClick(x,y);
    if(!safe)
        GameOver();
    event.Skip();
}

bool MyFrame::RegularClick(int x,int y)
{
    //Disable button 
    btn[y*nFieldWidth+x]->Enable(false);

    wxString label;
    if(mineField[y*nFieldWidth+x]== -1)
    {
        return false;
    }
    if(mineField[y*nFieldWidth+x]>0)
    {
        label.Printf("%d",mineField[y*nFieldWidth+x]);
        btn[y*nFieldWidth+x]->SetLabel(label);
        return true;
    }
    
    for (int i = -1 ; i < 2; i++)
    {
        for (int j = -1 ; j < 2; j++)
        {
            if(x+i>-1 && x+i<nFieldWidth && y+j>-1 && y+j<nFieldHeight)
            {
                if(btn[(y+j)*nFieldWidth+(x+i)]->IsEnabled())
                {
                    RegularClick(x+i,y+j);
                }
                else
                    continue;
            }
        }
    }   
    return true;
}

void MyFrame::FirstClick(int x, int y)
{
    int minesToInsert = this->numberOfMines;
    while(minesToInsert)
    {
        int rx = rand() % nFieldWidth;
        int ry = rand() % nFieldHeight;

        if(mineField[ry*nFieldWidth+rx] == 0 && rx != x && ry != y)
        {
            mineField[ry*nFieldWidth+rx] = -1;
            //btn[ry*nFieldWidth+rx]->SetLabel("B"); //DEBUG
            minesToInsert--;
        }
    }

    //Get number of neighboring bombs in the whole field
    for (int i = 0; i < nFieldWidth ; i++)
    {
        for (int j = 0; j < nFieldHeight; j++)
        {
            if(mineField[j*nFieldWidth+i] != -1)
            {
                mineField[j*nFieldWidth+i] = GetNumberOfNgbBombs(i,j); 
            }
        }
    }
    firstClick = false;
}

int MyFrame::GetNumberOfNgbBombs(int x, int y)
{
    int ngbBombs = 0;
    for (int i = -1 ; i < 2; i++)
    {
        for (int j = -1 ; j < 2; j++)
        {
            if(x+i>-1 && x+i<nFieldWidth && y+j>-1 && y+j<nFieldHeight)
            {
                if(this->mineField[(y+j)*nFieldWidth+(x+i)]==-1)
                {
                    ngbBombs++;
                }
            }
        }
    }
    return ngbBombs;
}

void MyFrame::GameOver()
{
    wxMessageBox("Game over!");

    //Reset game
    firstClick = true;
    this-> minesLeft = numberOfMines;
    for (int i = 0; i < nFieldWidth; i++)
    {
        for (int j = 0; j < nFieldHeight; j++)
        {
            mineField[j*nFieldWidth+i] = 0;
            btn[j*nFieldWidth+i]->Enable(true);
            btn[j*nFieldWidth+i]->SetLabel("");
        }
    }

}
void MyFrame::OnMarkField(wxMouseEvent& event)
{
    //Which button was clicked?
    int x = (event.GetId()-10000)%nFieldWidth;
    int y = (event.GetId()-10000)/nFieldHeight;
    
    wxString status;
    if(!isMarked[y*nFieldWidth+x])
    {
        btn[y*nFieldWidth+x]->SetLabel("M");
        //btn[y*nFieldWidth+x]->Enable(false);
        isMarked[y*nFieldWidth+x] = true;

        this->minesLeft--;
        status.Printf("Mines left: %d",this->minesLeft);
        SetStatusText(status);
    }
    else
    {
        btn[y*nFieldWidth+x]->SetLabel("");
        //btn[y*nFieldWidth+x]->Enable(true);
        isMarked[y*nFieldWidth+x] = false;

        this->minesLeft++;
        status.Printf("Mines left: %d",this->minesLeft);
        SetStatusText(status);
    }
    event.Skip();

}

