#include <boost/program_options.hpp>
#include <ModeloMame.h>
#include <ModeloLocalRoms.h>

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;
using namespace boost::program_options;

namespace po = boost::program_options;

class config
{
	enum mostrar {todo, correctos, incorrectos, inexistentes, mal_nombre};

public:
	string bdd;
	string version;

	int _mostar;
};

void procesar_parametros(int argc, char* argv[], string& fdb, string& version)
{
	po::options_description desc("Parametros Validos");
	desc.add_options()
		("ayuda,h", "Esta pantalla.")
		("help,h", "Esta pantalla.")
		("v1", "Version 1 - Validacion Mixta - NO USAR.")
		("v2", "Version 2 - Validacion en Base de Datos.")
		("v3", "Version 3 - Validacion en Memoria.")
		("bdd,b", po::value<string>(), "Base de Datos a guardar ListInfo.")
		;

	po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

	if (vm.size() == 0 || vm.count("ayuda")) cout << desc << endl;

	if (!vm.count("bdd")) throw string("Falta especificar la base de datos");
	else fdb = vm["bdd"].as<string>();

	if (vm.count("v1")) version = "v1";
	else if (vm.count("v2")) version = "v2";
	else if (vm.count("v3")) version = "v3";

	if (version.empty()) version = "v3";
}

void listar_juegos(const string& fdb)
{
	ModeloMame mame(fdb);

	vector<string> games=mame.sacar_games();

	for (vector<string>::iterator it=games.begin();it!=games.end();++it) {
		vector<rom> roms=mame.sacar_roms(*it);
		cout << *it << ":" << endl;
		for (vector<rom>::iterator it2=roms.begin();it2!=roms.end();++it2) {
			cout << "\t" << it2->nombre << " - " << it2->tamanio << " - " << it2->sha1 << " - " << it2->crc << endl;
		}
	}
}

void verificar_games(const string& fdb)
{
	ModeloMame mame(fdb);

	vector<string> games=mame.sacar_games();

//	games.sort();

	for (vector<string>::iterator it=games.begin();it!=games.end();++it) {
		vector<rom> roms=mame.sacar_roms(*it);
		vector<rom> mis_roms=mame.sacar_mi_roms(*it + ".zip");

		if (mis_roms.empty()) {cout << endl << *it << ": Inexistente"; continue;};

		vector<rom>::iterator it_roms;
		vector<rom>::iterator it_mis_roms;

		cout << endl << *it << ": ";
		for (it_roms=roms.begin();it_roms!=roms.end();++it_roms) {
			bool existe=false;
			for (it_mis_roms=mis_roms.begin();it_mis_roms!=mis_roms.end();++it_mis_roms) {
				if (*it_roms != *it_mis_roms) {
					if (it_roms->comparar_sin_nombre(*it_mis_roms))
						cout << endl << "\t"<< it_roms->nombre << " Cambio de nombre a " << it_mis_roms->nombre;
				} else {
						// cout << "\t"<< it_roms->nombre << " Correcto" << endl;
						mis_roms.erase(it_mis_roms);
						existe=true;
						break;
				}
			}

			if (!existe)
				{
				pair<string,string> faltante;
				faltante = mame.buscar_mi_rom("","",it_roms->tamanio, it_roms->crc, it_roms->sha1);

				if (!faltante.first.empty())
					{
					cout << endl << "\tExiste en " << faltante.first << " --> " << faltante.second;
					}
				else
					{
					cout << endl << "\tfalta " << it_roms->nombre;
					mis_roms.push_back(rom());
					}
				}
		}

		if (mis_roms.empty()) cout << "Correcto " << endl;

/*
		bool igual=true;

		for (it_roms=roms.begin(),it_mis_roms=mis_roms.begin();it_roms!=roms.end();++it_roms) {
			if (*it_roms != *it_mis_roms) {
				igual=false;
				break;
			}
		}

		if (!igual)
			cout << *it << " no se corresponde ..." << endl;
		else
			cout << *it << " son IGUALES ..." << endl;
*/
	}
}

void verificar_gamesV2(const string& fdb)
{
	ModeloMame mame(fdb);

	vector<string> games=mame.sacar_games();

//	games.sort();

	for (vector<string>::iterator it=games.begin();it!=games.end();++it) {
		vector<rom> roms=mame.sacar_roms(*it);
		vector<rom> mis_roms=mame.sacar_mi_roms(*it + ".zip");

		if (mis_roms.empty()) {cout << endl << *it << ": No Existe"; continue;};

		cout << endl << *it << ": ";
		unsigned int total_archivos=0;
		for (vector<rom>::iterator it_roms=roms.begin();it_roms!=roms.end();++it_roms) {
			pair<string,string> existente;
			existente = mame.buscar_mi_rom(*it + ".zip", it_roms->nombre, it_roms->tamanio, it_roms->crc, it_roms->sha1);

			if (existente.first.empty()){
				// Busco si cambio de nombre en mismo archivo .zip
				existente = mame.buscar_mi_rom(*it + ".zip", "", it_roms->tamanio, it_roms->crc, it_roms->sha1);
				if (existente.first.empty()) {
					// Busco si existe en otro archivo zip
					existente = mame.buscar_mi_rom("", it_roms->nombre, it_roms->tamanio, it_roms->crc, it_roms->sha1);
					if (existente.first.empty()) {
						// Busco si existe en otro archivo zip con distinto nombre
						existente = mame.buscar_mi_rom("", "", it_roms->tamanio, it_roms->crc, it_roms->sha1);
						if (existente.first.empty()){
							cout << "\n\t" << it_roms->nombre << ": Archivo no encontrado ";
						} else {cout << "\n\t" << it_roms->nombre << ": Archivo existe en " << existente.first << " con el nombre " << existente.second;}
					} else {cout << "\n\t" << it_roms->nombre << ": Archivo existe en " << existente.first;}
				} else {cout << "\n\t" << it_roms->nombre << ": Tiene el nombre " << existente.second;}
			} else { total_archivos++;continue;}
		}

		if (total_archivos == roms.size())
				cout << "Correcto";
	}

	cout << endl;
}

void verificar_gamesV3(const string& fdb)
{
	ModeloMame mame(fdb);
	ModeloLocalRoms misroms("../bdd/misroms.fdb");

	map<string,vector<rom> > catalogo_roms;
	mame.roms2map(catalogo_roms);

	vector<string> games=mame.sacar_games();
//	games.sort();

	map<string,vector<rom> > catalogo_mis_roms;
	misroms.roms2map(catalogo_mis_roms);

//	map<string, list<rom> >::iterator it_gamesroms=catalogo_roms.find("calspeda");

	for (vector<string>::iterator it=games.begin();it!=games.end();++it) {
		map<string, vector<rom> >::iterator it_gamesroms=catalogo_roms.find(*it);
		if (it_gamesroms == catalogo_roms.end()) {
			cout << endl << *it << ": No Existe";
			continue;
		};

		vector<rom> roms=it_gamesroms->second;

		// busco el nombre del archivo dentro de mis juegos.
		map<string, vector<rom> >::iterator it_juegosroms=catalogo_mis_roms.find(*it + ".zip");
		if (it_juegosroms == catalogo_mis_roms.end()) {cout << endl << *it << ": No Existe"; continue;};

		vector<rom> mis_roms=it_juegosroms->second;

		cout << endl << *it << ": ";
		unsigned int total_archivos=0;
		for (vector<rom>::iterator it_roms=roms.begin();it_roms!=roms.end();++it_roms) {
			EncuentraRom mis_utils(*it_roms);
			if (!mis_utils.existe(mis_roms)){
				// Busco si cambio de nombre en mismo archivo .zip
				string archivo = mis_utils.buscaHash(mis_roms).nombre;
				if (archivo.empty()) {
					pair<string,string> existente;
					// Busco si existe en otro archivo zip
					archivo = mis_utils.buscaRom(catalogo_mis_roms);
					if (archivo.empty()) {
						// Busco si existe en otro archivo zip con distinto nombre
						existente = mis_utils.buscaHash(catalogo_mis_roms);
						if (existente.first.empty()){
							cout << "\n\t" << it_roms->nombre << ": Archivo no encontrado ";
						} else {cout << "\n\t" << it_roms->nombre << ": Archivo existe en " << existente.first << " con el nombre " << existente.second;}
					} else {cout << "\n\t" << it_roms->nombre << ": Archivo existe en " << archivo;}
				} else {cout << "\n\t" << it_roms->nombre << ": Tiene el nombre " << archivo;}
			} else { total_archivos++;continue;}
		}

		if (total_archivos == roms.size())
				cout << "Correcto";
	}

	cout << endl;
}

int main(int argc, char* argv[])
{
	string fdb;
	string version;

	try{
		procesar_parametros(argc, argv, fdb, version);

		if (version.compare("v1") == 0)
			cout << "NO Utilizar!!!" << endl;
		else if (version.compare("v2") == 0)
			verificar_gamesV2(fdb);
		else if (version.compare("v3") == 0)
			verificar_gamesV3(fdb);

	}catch (const string& e) {cout << e << endl;};

	return 0;
}
