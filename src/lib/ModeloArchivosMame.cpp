#include "ModeloArchivosMame.h"

void ModeloArchivosMame::cargar_directorio(const string& directorio, map<string, vector<rom> >& mapa_directorio)
{
	mapa_directorio.clear();

	for (ModeloArchivosMame::iterator ita=begin(directorio);ita != end();++ita) {
		cout << "Analizado " << ita.nombre() << endl;
		mapa_directorio.insert(make_pair(ita.nombre(), ita.contenido()));
	}
}

vector<rom>& ModeloArchivosMame::contenido_zip(const string& archivozip, vector<rom>& contenidozip)
{
	GuruUnzip unzip(archivozip);

	map<string, unz_file_info> contenido;

	try {
		contenidozip.clear();

		unzip.contenido(contenido);

		unz_file_info data;
		for (map<string, unz_file_info>::iterator itc=contenido.begin();itc!=contenido.end();++itc) {
			rom _rom;
			data = itc->second;			

			_rom.nombre = itc->first;
			_rom.tamanio = data.uncompressed_size;
			_rom.crc = to_lower_copy(GuruCRC::dword_a_string(data.crc));

			GuruBuffer buf(data.uncompressed_size);
			unzip.extraer(itc->first, buf);
			if (buf.buf() == NULL)
				_rom.sha1 = "0000000000000000000000000000000000000000";
			else
				_rom.sha1 = to_lower_copy(GuruSHA1::sha1((byte*)buf.buf(), data.uncompressed_size));

			contenidozip.push_back(_rom);
		}
	} catch (...) {}

	return contenidozip;
}
