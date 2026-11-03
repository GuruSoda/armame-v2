#ifndef __VistaVerificar__
#define __VistaVerificar__

#include <iostream>
#include <ModeloArchivosMame.h>
#include <GuruMameUtil.h>

#include "VerificarGUI.h"

#include <boost/algorithm/string/erase.hpp>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

/** Implementing VerificaRom */
class VistaVerificar : public VerificaRom
{
protected:
	// Handlers for VerificaRom events.
	void CheckMisRoms( wxCommandEvent& event );
	void ClickVerificar( wxCommandEvent& event );
	void ClickSalir( wxCommandEvent& event );
	
public:
	/** Constructor */
	VistaVerificar( wxWindow* parent );
};

#endif // __VistaVerificar__
