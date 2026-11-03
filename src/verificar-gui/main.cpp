#include "VistaVerificar.h"
#include "wx/wx.h"

class wxWidgetsApp : public wxApp
{
public:
    wxWidgetsApp() {} ;
    virtual ~wxWidgetsApp() {} ;
    virtual bool OnInit();
};

bool wxWidgetsApp::OnInit()
{
//	wxInitAllImageHandlers();
	wxImageHandler* jpegLoader = new wxICOHandler();
	wxImage::AddHandler(jpegLoader);

	VistaVerificar* dialog = new VistaVerificar( (wxWindow*)NULL );
    dialog->Show();
    SetTopWindow( dialog );
    return true;
}

DECLARE_APP(wxWidgetsApp)

IMPLEMENT_APP(wxWidgetsApp)
