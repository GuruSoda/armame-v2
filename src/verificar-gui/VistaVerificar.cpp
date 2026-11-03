#include "VistaVerificar.h"

VistaVerificar::VistaVerificar( wxWindow* parent ):VerificaRom(parent)
{
	wxListItem item;
	item.SetText(wxT("ROM"));
	mListaRoms->InsertColumn(0, item);
	mListaRoms->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);

	item.SetText(wxT("Estado"));
	mListaRoms->InsertColumn(1, item);
	mListaRoms->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);

//	mArchivoMame->SetPath(wxT("/opt/ArMame2/bdd/listinfo.fdb"));
}

void VistaVerificar::CheckMisRoms( wxCommandEvent& event )
{
	if (mHabilitarRomsLocales->IsChecked())
		mArchivoMisRoms->Enable(true);
	else
		mArchivoMisRoms->Enable(false);
}

void VistaVerificar::ClickVerificar( wxCommandEvent& event )
{
	mListaRoms->DeleteAllItems();

	ModeloArchivosMame 	zips;

	try{
		vector<rom> rom_archivo;
		zips.contenido_zip(string(mArchivoZip->GetPath().ToAscii()), rom_archivo);
		path ruta(string(mArchivoZip->GetPath().ToAscii()));

		string juego_local(ierase_last_copy(to_lower_copy(ruta.filename()), ".zip"));

		VerificaRoms cr(string(mArchivoMame->GetPath().ToAscii()));
		vector<pair<rom, VerificaRoms::estado> > resultado;

		if (cr.verificar(juego_local, rom_archivo, resultado) == 0)
			mBarraEstado->SetStatusText(wxT("Correcto"));
		else {
			mBarraEstado->SetStatusText(wxT("INCorrecto"));
			int indice=0;
			for (vector<pair<rom, VerificaRoms::estado> >::iterator itr=resultado.begin();itr!=resultado.end();++itr, indice++){
				mListaRoms->InsertItem(indice, -1);
				switch (itr->second.estadoRom()){
				case VerificaRoms::igual:
					mListaRoms->SetItem(indice, 0, wxString (itr->first.nombre.c_str(), wxConvUTF8));
					mListaRoms->SetItem(indice, 1, wxT("Correcta"));
					break;
				case VerificaRoms::falta:
					mListaRoms->SetItem(indice, 0, wxString (itr->first.nombre.c_str(), wxConvUTF8));
					mListaRoms->SetItem(indice, 1, wxT("Falta"));
					break;
				case VerificaRoms::distinto_nombre:
					mListaRoms->SetItem(indice, 0, wxString (itr->first.nombre.c_str(), wxConvUTF8));
					mListaRoms->SetItem(indice, 1, wxT("Otro Nombre"));
					// cout << "\t" << itr->first.nombre << ": Tiene el nombre " << itr->second.nombre() << endl;
					break;
				default:
					cout << "\t" << itr->first.nombre << ": Desconocida?" << endl;
				}
			}
		}
	}catch (std::string& e){
		cout << e << endl;
	}
}

void VistaVerificar::ClickSalir( wxCommandEvent& event )
{
	Close();
}
