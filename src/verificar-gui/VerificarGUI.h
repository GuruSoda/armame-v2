///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __VerificarGUI__
#define __VerificarGUI__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/filepicker.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/listctrl.h>
#include <wx/panel.h>
#include <wx/statusbr.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class VerificaRom
///////////////////////////////////////////////////////////////////////////////
class VerificaRom : public wxFrame 
{
	private:
	
	protected:
		wxPanel* m_panel3;
		wxStaticText* mTextArchivo;
		wxFilePickerCtrl* mArchivoZip;
		wxCheckBox* mHabilitarRomsLocales;
		wxStaticText* mTextBddMame;
		wxFilePickerCtrl* mArchivoMame;
		wxFilePickerCtrl* mArchivoMisRoms;
		wxButton* m_button3;
		wxButton* m_button4;
		wxButton* m_button5;
		wxListCtrl* mListaRoms;
		wxButton* mBotonVerificar;
		wxButton* mBotonSalir;
		wxStatusBar* mBarraEstado;
		
		// Virtual event handlers, overide them in your derived class
		virtual void CheckMisRoms( wxCommandEvent& event ){ event.Skip(); }
		virtual void ClickRepararNombres( wxCommandEvent& event ){ event.Skip(); }
		virtual void ClickAgregarRoms( wxCommandEvent& event ){ event.Skip(); }
		virtual void ClickVerificar( wxCommandEvent& event ){ event.Skip(); }
		virtual void ClickSalir( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		VerificaRom( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Verificacion de Roms"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 550,350 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		~VerificaRom();
	
};

#endif //__VerificarGUI__
