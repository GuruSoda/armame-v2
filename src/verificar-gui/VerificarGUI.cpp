///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 29 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "VerificarGUI.h"

///////////////////////////////////////////////////////////////////////////

VerificaRom::VerificaRom( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_panel3 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableCol( 4 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	mTextArchivo = new wxStaticText( m_panel3, wxID_ANY, wxT("Archivo:"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	mTextArchivo->Wrap( -1 );
	fgSizer1->Add( mTextArchivo, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	mArchivoZip = new wxFilePickerCtrl( m_panel3, wxID_ANY, wxT("/opt/roms"), wxT("Selecciones un Archivo a Verificar"), wxT("*.zip"), wxPoint( -1,-1 ), wxDefaultSize, wxFLP_DEFAULT_STYLE );
	fgSizer1->Add( mArchivoZip, 0, wxALL|wxEXPAND, 5 );
	
	mHabilitarRomsLocales = new wxCheckBox( m_panel3, wxID_ANY, wxT("Habilitar Base con roms locales"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer1->Add( mHabilitarRomsLocales, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	mTextBddMame = new wxStaticText( m_panel3, wxID_ANY, wxT("FDB Mame:"), wxPoint( -1,-1 ), wxDefaultSize, 0 );
	mTextBddMame->Wrap( -1 );
	fgSizer1->Add( mTextBddMame, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	mArchivoMame = new wxFilePickerCtrl( m_panel3, wxID_ANY, wxT("/opt/ArMame2/bdd/listinfo.fdb"), wxT("Seleccione la Base de Datos del M.A.M.E."), wxT("*.fdb"), wxPoint( -1,-1 ), wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_USE_TEXTCTRL );
	fgSizer1->Add( mArchivoMame, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	mArchivoMisRoms = new wxFilePickerCtrl( m_panel3, wxID_ANY, wxEmptyString, wxT("Select a file"), wxT("*.fdb"), wxDefaultPosition, wxDefaultSize, wxFLP_DEFAULT_STYLE|wxFLP_FILE_MUST_EXIST|wxFLP_USE_TEXTCTRL );
	mArchivoMisRoms->Enable( false );
	
	fgSizer1->Add( mArchivoMisRoms, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	bSizer8->Add( fgSizer1, 0, wxEXPAND|wxALL, 5 );
	
	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button3 = new wxButton( m_panel3, wxID_ANY, wxT("Reparar Nombres"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer51->Add( m_button3, 0, wxALL, 5 );
	
	m_button4 = new wxButton( m_panel3, wxID_ANY, wxT("Agregar Roms Faltantes"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer51->Add( m_button4, 0, wxALL, 5 );
	
	m_button5 = new wxButton( m_panel3, wxID_ANY, wxT("No hacer nada"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer51->Add( m_button5, 0, wxALL, 5 );
	
	bSizer8->Add( bSizer51, 0, wxALIGN_CENTER_HORIZONTAL, 5 );
	
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );
	
	mListaRoms = new wxListCtrl( m_panel3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_HRULES|wxLC_REPORT );
	bSizer7->Add( mListaRoms, 1, wxEXPAND|wxALL, 5 );
	
	bSizer8->Add( bSizer7, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	mBotonVerificar = new wxButton( m_panel3, wxID_ANY, wxT("&Verificar"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( mBotonVerificar, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	mBotonSalir = new wxButton( m_panel3, wxID_ANY, wxT("&Salir"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( mBotonSalir, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
	
	bSizer8->Add( bSizer5, 0, wxALIGN_RIGHT, 5 );
	
	m_panel3->SetSizer( bSizer8 );
	m_panel3->Layout();
	bSizer8->Fit( m_panel3 );
	bSizer4->Add( m_panel3, 1, wxEXPAND, 5 );
	
	this->SetSizer( bSizer4 );
	this->Layout();
	mBarraEstado = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );
	
	// Connect Events
	mHabilitarRomsLocales->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( VerificaRom::CheckMisRoms ), NULL, this );
	m_button3->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VerificaRom::ClickRepararNombres ), NULL, this );
	m_button4->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VerificaRom::ClickAgregarRoms ), NULL, this );
	mBotonVerificar->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VerificaRom::ClickVerificar ), NULL, this );
	mBotonSalir->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VerificaRom::ClickSalir ), NULL, this );
}

VerificaRom::~VerificaRom()
{
	// Disconnect Events
	mHabilitarRomsLocales->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( VerificaRom::CheckMisRoms ), NULL, this );
	m_button3->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VerificaRom::ClickRepararNombres ), NULL, this );
	m_button4->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VerificaRom::ClickAgregarRoms ), NULL, this );
	mBotonVerificar->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VerificaRom::ClickVerificar ), NULL, this );
	mBotonSalir->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( VerificaRom::ClickSalir ), NULL, this );
}
