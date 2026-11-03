#include <boost/program_options.hpp>
#include <ModeloMame.h>
#include <ModeloLocalRoms.h>
#include <GuruMameUtil.h>
#include <VerificaJuego.h>

#include <boost/algorithm/string/erase.hpp>

using namespace std;
namespace po = boost::program_options;

void mameVSlocal (const string& fdb_mame, const string& fdb_local);
void mameVSlocal2 (const string& fdb_mame, const string& fdb_local);
void localVSmame (const string& fdb_mame, const string& fdb_local);

int main(int argc, char* argv[])
{
	try{
		po::options_description desc("Parametros Validos");
		desc.add_options()
			("ayuda,h", "Esta pantalla.")
			("mame,m", po::value<string>(), "Base de Datos M.A.M.E..")
			("local,l", po::value<string>(), "Base de Datos local.")
			("vm", "Verifica la Base de Datos del M.A.M.E. contra la local.")
			("vl", "Verifica la Base de Datos Local contra la del M.A.M.E..")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.size() == 0 || vm.count("ayuda")) cout << desc << endl;

		string fdb_mame, fdb_local;

		if (!vm.count("mame")) throw string("Falta especificar la base de datos M.A.M.E..");
		else fdb_mame = vm["mame"].as<string>();

		if (!vm.count("local")) throw string("Falta especificar la base de datos Local.");
		else fdb_local = vm["local"].as<string>();

		time_t inicio,fin;
		time(&inicio);

		if (vm.count("vm")){
			mameVSlocal2 (fdb_mame, fdb_local);
			time(&fin);
			cout << "Segundos transcurridos:" << (fin-inicio) << endl;
			return 0;
		}

		if (vm.count("vl")){
			localVSmame (fdb_mame, fdb_local);
			time(&fin);
			cout << "Segundos transcurridos:" << (fin-inicio) << endl;
			return 0;
		}

		cout << "Que desea hacer?" << endl;

	}catch (string& e) {
		cerr << e << endl;
	}

	return 0;
}

void mameVSlocal2 (const string& fdb_mame, const string& fdb_local)
{
	ModeloMame _mame(fdb_mame);
	ModeloLocalRoms _localroms(fdb_local);

	VerificaJuego veri(_mame, _localroms, VerificaJuego::memoria);

	vector <string> juegos;
	juegos = _mame.sacar_games();

	for (vector<string>::iterator it=juegos.begin();it!=juegos.end();++it){
			vector<pair<rom, VerificaJuego::EstadoRom> > resultado;

			try{
				int estado=veri.mameVSlocalroms(*it, resultado);

				cout << *it << ":" << ((estado) ? "Incorrecto" : "Correcto") << endl;

				if (estado == 0) continue;

				for (vector<pair<rom, VerificaJuego::EstadoRom> >::iterator itr=resultado.begin();itr!=resultado.end();++itr){
					switch (itr->second.estadoRom()) {
					case VerificaJuego::igual:
						// cout << "\t" << itr->first.nombre << ": Correcta." << endl;
						break;
					case VerificaJuego::falta:
						cout << "\t" << itr->first.nombre << ": Falta" << endl;
						break;
					case VerificaJuego::distinto_nombre:
						cout << "\t" << itr->first.nombre << ": Tiene el nombre " << itr->second.nombre() << endl;
						break;
					case VerificaJuego::distinto_hash:
						cout << "\t" << itr->first.nombre << ": Tiene distinto hash " << itr->second.nombre() << endl;
						break;
					case VerificaJuego::otro_archivo:
						cout << "\t" << itr->first.nombre << ": Esta en el archivo " << itr->second.archivo() << endl;
						break;
					case VerificaJuego::otro_archivo_nombre:
						cout << "\t" << itr->first.nombre << ": Esta en el archivo " << itr->second.archivo() << " con el nombre " << itr->second.nombre() << endl;
						break;
					default:
						cout << "\t" << itr->first.nombre << ": Desconocida?" << endl;
					}
				}

			}catch (string& e) {cout << *it + ": "<< e << endl;}
	}
}

void mameVSlocal (const string& fdb_mame, const string& fdb_local)
{
	ModeloMame		mame(fdb_mame);
	ModeloLocalRoms local(fdb_local);

	map<string,vector<rom> > catalogolocal;
	map<string,vector<rom> > catalogomame;
	
	local.roms2map(catalogolocal);
	mame.roms2map(catalogomame);

	// comienzo a recorrer cada juego del mame
	for (map<string,vector<rom> >::iterator it=catalogomame.begin();it!=catalogomame.end();it++){
		string juego(it->first);

		cout << "Analizando: " << juego;
		// saco el juego desde mi catalogo local
		map<string,vector<rom> >::iterator itj=catalogolocal.find(to_lower_copy(it->first + ".zip"));
		if (itj==catalogolocal.end()) {
			cout << " - No existe el archivo del juego " << it->first << endl;
			continue;
		} else cout << endl;

		// cargo el juego sacado del mame
		vector<rom> roms_juego_mame(it->second);

		// cargo las roms del juego local
		vector<rom> roms_juego_local(itj->second);

		rom mirom;
		// comienzo a recorrer cada rom del mame
		for (vector<rom>::iterator it2=roms_juego_mame.begin();it2!=roms_juego_mame.end();++it2){
			EncuentraRom buscar(*it2);

			if (buscar.existe(roms_juego_local))
				continue;

			if ((mirom=buscar.buscaHash(roms_juego_local)).tamanio!=0) {
				cout << "\t" << it2->nombre << ": Tiene el nombre " << mirom.nombre << endl; 
				continue;
			}

			string nombre_juego;
			if (!(nombre_juego=buscar.buscaRom(catalogolocal)).empty()) {
				cout << "\t" << it2->nombre << ": Esta en el archivo " << nombre_juego << endl;
				continue;
			}

			pair<string, string> lugar;
			if (!(lugar=buscar.buscaHash(catalogolocal)).first.empty()){
				cout << "\t" << it2->nombre << ": Esta en el archivo " << lugar.first << " con el nombre " << lugar.second << endl;
				continue;
			}

/*
///////////////// Realizado integramente con funciones de STL //////////////////////////////////
			vector<rom>::iterator itmame;
			map<string,vector<rom> >::iterator itcatmame;

			// busco el archivo exacto(nombre, crc, sha1, etc)
			itmame =find_if(roms_juego_mame.begin(), roms_juego_mame.end(), ExisteRom(*it2));
			if (itmame!=roms_juego_mame.end()) continue;			
			
			// busco por el hash en el mismo archivo, puede tener otro nombre.
			itmame=find_if(roms_juego_mame.begin(), roms_juego_mame.end(), ExisteHash(*it2));
			if (itmame!=roms_juego_mame.end()) {cout << "Tiene el nombre:" << itmame->nombre << endl;continue;}

			// busco el archivo por si se encuentra en otro archivo
			itcatmame=find_if(catalogolocal.begin(), catalogolocal.end(), BuscaRom(*it2));
			if (itmame!=roms_juego_mame.end()) {cout << "Esta en el archivo:" << itcatmame->first << endl;continue;}

			// busco por el hash y si esta en otro archivo
			itcatmame=find_if(catalogolocal.begin(), catalogolocal.end(), BuscaHash(*it2));
			if (itmame!=roms_juego_mame.end()) {cout << "Esta en el archivo:" << itcatmame->first << " con otro nombre" << endl;continue;}
*/

			cout << "\t" << it2->nombre << ": No existe." << endl;
		} // for de roms
	} // for de juegos
}

void localVSmame (const string& fdb_mame, const string& fdb_local)
{
	ModeloLocalRoms local(fdb_local);
	ModeloMame		mame(fdb_mame);

	map<string,vector<rom> > catalogolocal;
	map<string,vector<rom> > catalogomame;
	
	local.roms2map(catalogolocal);
	mame.roms2map(catalogomame);

	// comienzo a recorrer cada juego que tengo
	for (map<string,vector<rom> >::iterator it=catalogolocal.begin();it!=catalogolocal.end();it++){
		string juego(ierase_last_copy(to_lower_copy(it->first), ".zip"));

		cout << "Analizando: " << juego;
		// saco el juego desde el mame
		map<string,vector<rom> >::iterator itj=catalogomame.find(juego);
		if (itj==catalogomame.end()) {
			cout << " - No existe en juego llamado " << juego << " en el mame." << endl;
			continue;
		} else cout << endl;

		// cargo el juego sacado del mame
		vector<rom> roms_juego_mame(itj->second);

		// cargo las roms del juego local
		vector<rom> roms_juego_local(it->second);

		rom mirom;
		// comienzo a recorrer cada rom local
		for (vector<rom>::iterator it2=roms_juego_local.begin();it2!=roms_juego_local.end();++it2){
			EncuentraRom buscar(*it2);

			if (buscar.existe(roms_juego_mame))
				continue;

			if ((mirom=buscar.buscaHash(roms_juego_mame)).tamanio!=0) {
				cout << "\t" << it2->nombre << ": Debe tener el nombre " << mirom.nombre << endl;
				continue;
			}

			string nombre_juego;
			if (!(nombre_juego=buscar.buscaRom(catalogomame)).empty()) {
				cout << "\t" << it2->nombre << ": Es del juego " << nombre_juego << endl;
				continue;
			}

			pair<string, string> lugar;
			if (!(lugar=buscar.buscaHash(catalogomame)).first.empty()){
				cout << "\t" << it2->nombre << ": Es del juego " << lugar.first << " con el nombre " << lugar.second << endl;
				continue;
			}

/*
///////////////// Realizado integramente con funciones de STL //////////////////////////////////
			vector<rom>::iterator itmame;
			map<string,vector<rom> >::iterator itcatmame;

			// busco el archivo exacto(nombre, crc, sha1, etc)
			itmame =find_if(roms_juego_mame.begin(), roms_juego_mame.end(), ExisteRom(*it2));
			if (itmame!=roms_juego_mame.end()) continue;			
			
			// busco por el hash en el mismo archivo, puede tener otro nombre.
			itmame=find_if(roms_juego_mame.begin(), roms_juego_mame.end(), ExisteHash(*it2));
			if (itmame!=roms_juego_mame.end()) {cout << "Tiene el nombre:" << itmame->nombre << endl;continue;}

			// busco el archivo por si se encuentra en otro archivo
			itcatmame=find_if(catalogolocal.begin(), catalogolocal.end(), BuscaRom(*it2));
			if (itmame!=roms_juego_mame.end()) {cout << "Esta en el archivo:" << itcatmame->first << endl;continue;}

			// busco por el hash y si esta en otro archivo
			itcatmame=find_if(catalogolocal.begin(), catalogolocal.end(), BuscaHash(*it2));
			if (itmame!=roms_juego_mame.end()) {cout << "Esta en el archivo:" << itcatmame->first << " con otro nombre" << endl;continue;}
*/

			cout << "\t" << it2->nombre << ": No es un archivo valido." << endl;
		} // for de roms
	} // for de juegos
}
