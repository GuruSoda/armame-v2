#include "VerificaJuego.h"

int VerificaJuego::mameVSlocalroms(const string& juego, vector<rom>& mis_roms, vector<pair<rom, EstadoRom> >& resultado)
{
	int incorrectos = 0;

	vector<rom> roms_juego = _mame.sacar_roms(juego);

	if (roms_juego.size() == 0) throw string("Juego Inexistente.");

	resultado.clear();
	for (vector<rom>::iterator it=roms_juego.begin();it!=roms_juego.end();++it) {
			vector<rom>::iterator itmame;

			// busco el archivo exacto(nombre, crc, sha1, etc)
			itmame =find_if(mis_roms.begin(), mis_roms.end(), ExisteRom(*it));
			if (itmame!=mis_roms.end()){
				resultado.push_back(make_pair(*it, EstadoRom("","", igual)));
				continue;
			}

			// busco por el hash en el mismo archivo, puede tener otro nombre.
			itmame = find_if(mis_roms.begin(),mis_roms.end(), ExisteHash(*it));
			if (itmame!=mis_roms.end()) {
				resultado.push_back(make_pair(*it, EstadoRom(itmame->nombre,"", distinto_nombre)));
				incorrectos++;
				continue;
			}

			if (!_localroms.bdd().empty()){
				pair<string, string> res;
				res=_localroms.buscar_rom("", "", it->tamanio, it->crc, it->sha1);
				// first = nombre del archivo .zip con la ruta incluida.
				// second = nombre de la rom encontrada.

				// pregunto si hay datos donde va el archivo zip donde encontre la rom
				if (!res.first.empty()){
					if (res.second.compare(it->nombre) == 0) // tiene el mismo nombre que el que busco?
						resultado.push_back(make_pair(*it, EstadoRom(res.second,res.first, otro_archivo)));
					else
						resultado.push_back(make_pair(*it, EstadoRom(res.second,res.first, otro_archivo_nombre)));

					incorrectos++;
					continue;
				}
			}

			resultado.push_back(make_pair(*it, EstadoRom("","", falta)));
			incorrectos++;
	}

	return incorrectos;
}

int VerificaJuego::mameVSlocalroms(const string& juego, vector<pair<rom, EstadoRom> >& resultado)
{
	int incorrectos = 0;

	if (tipoBusqueda == memoria && catalogomame.empty()) {
		_localroms.roms2map(catalogolocalroms);
		_mame.roms2map(catalogomame);
	}

	vector<rom> roms_juego;

	if (tipoBusqueda == bdd)
		roms_juego = _mame.sacar_roms(juego);
	else if (tipoBusqueda == memoria){
		map<string,vector<rom> >::iterator it=catalogomame.find(to_lower_copy(juego));
		if (it != catalogomame.end()) roms_juego = it->second;
	}

	if (roms_juego.size() == 0) throw string("Juego Mame Inexistente.");

	resultado.clear();

	// first = nombre del archivo .zip con la ruta incluida.
	// second = nombre de la rom encontrada.
	pair<string, string> lugar_rom;
	vector<rom> roms_locales;

	if (tipoBusqueda == bdd)
		_localroms.sacar_roms(juego + ".zip", "", roms_locales);
	else if (tipoBusqueda == memoria) {
		map<string,vector<rom> >::iterator it=catalogolocalroms.find(to_lower_copy(juego + ".zip"));
		if (it != catalogolocalroms.end()) roms_locales = it->second;
	}

	if (roms_locales.size() == 0) throw string("Juego Local Inexistente.");

	for (vector<rom>::iterator it=roms_juego.begin();it!=roms_juego.end();++it) {

			vector<rom>::iterator itmame;

			// busco el archivo exacto(nombre, crc, sha1, etc)
			itmame =find_if(roms_locales.begin(), roms_locales.end(), ExisteRom(*it));
			if (itmame!=roms_locales.end()){
				resultado.push_back(make_pair(*it, EstadoRom("","", igual)));
				continue;
			}

			// busco por el hash en el mismo archivo, puede tener otro nombre.
			itmame = find_if(roms_locales.begin(),roms_locales.end(), ExisteHash(*it));
			if (itmame!=roms_locales.end()) {
				resultado.push_back(make_pair(*it, EstadoRom(itmame->nombre,"", distinto_nombre)));
				incorrectos++;
				continue;
			}

			// busco si la rom esta en otro archivo distinto al que tiene estar
			if (tipoBusqueda == bdd)
				lugar_rom=_localroms.buscar_rom("", "", it->tamanio, it->crc, it->sha1);
			else if (tipoBusqueda == memoria)
				lugar_rom=EncuentraRom(*it).buscaHash(catalogolocalroms);

			// pregunto si hay datos donde va el archivo zip donde encontre la rom
			if (!lugar_rom.first.empty()){
				if (lugar_rom.second.compare(it->nombre) == 0) // tiene el mismo nombre que el que busco?
					resultado.push_back(make_pair(*it, EstadoRom(lugar_rom.second,lugar_rom.first, otro_archivo)));
				else
					resultado.push_back(make_pair(*it, EstadoRom(lugar_rom.second,lugar_rom.first, otro_archivo_nombre)));

				incorrectos++;
				continue;
			}

			// busco solo el nombre del archivo
			itmame = find_if(roms_locales.begin(),roms_locales.end(), ExisteNombre(*it));
			if (itmame!=roms_locales.end()) {
				resultado.push_back(make_pair(*it, EstadoRom("","", distinto_hash)));
				incorrectos++;
				continue;
			}
/*
			// busco el archivo exacto(nombre, crc, sha1, etc)
			lugar_rom=_localroms.buscar_rom(juego + ".zip", it->nombre, it->tamanio, it->crc, it->sha1);
			if (!lugar_rom.first.empty()){
				resultado.push_back(make_pair(*it, EstadoRom("","", igual)));
				continue;
			}

			// busco solo el nombre del archivo
			lugar_rom=_localroms.buscar_rom(juego + ".zip", it->nombre, -1, "", "");
			if (!lugar_rom.first.empty()){
				resultado.push_back(make_pair(*it, EstadoRom("","", distinto_hash)));
				continue;
			}

			// busco por el hash en el mismo archivo, puede tener otro nombre.
			lugar_rom=_localroms.buscar_rom(juego + ".zip", "", it->tamanio, it->crc, it->sha1);
			if (!lugar_rom.first.empty()){
				resultado.push_back(make_pair(*it, EstadoRom(itmame->nombre,"", distinto_nombre)));
				incorrectos++;
				continue;
			}

			// busco si la rom esta en otro archivo distinto al que tiene estar
			lugar_rom=_localroms.buscar_rom("", "", it->tamanio, it->crc, it->sha1);
			// pregunto si hay datos donde va el archivo zip donde encontre la rom
			if (!lugar_rom.first.empty()){
				if (lugar_rom.second.compare(it->nombre) == 0) // tiene el mismo nombre que el que busco?
					resultado.push_back(make_pair(*it, EstadoRom(lugar_rom.second,lugar_rom.first, otro_archivo)));
				else
					resultado.push_back(make_pair(*it, EstadoRom(lugar_rom.second,lugar_rom.first, otro_archivo_nombre)));

				incorrectos++;
				continue;
			}
*/
			resultado.push_back(make_pair(*it, EstadoRom("","", falta)));
			incorrectos++;
	}

	return incorrectos;
}

void VerificaJuego::localromsVSmame()
{

}
