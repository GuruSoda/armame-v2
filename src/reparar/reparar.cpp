#include <boost/program_options.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/filesystem.hpp>
#include <ModeloArchivosMame.h>
#include <GuruMameUtil.h>
#include <GuruZip.h>
#include <GuruUnzip.h>

using namespace boost::program_options;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace std;

namespace po = boost::program_options;

void reparar_archivo (const string& bdd, const string& archivo, const string& romlocales);
void reparar_directorio (const string& bdd, const string& directorio, const string& romlocales);
void reparar_zip(const string& archivo, vector<pair<rom, VerificaRoms::estado> >& reparacion);

int main (int argc, char* argv[])
{
	try{
		po::options_description desc("Parametros Validos");
		desc.add_options()
			("ayuda,h", "Esta pantalla.")
			("bdd,b", po::value<string>(), "Base de Datos del M.A.M.E..")
			("local,l", po::value<string>(), "Base de Datos de Roms Locales (Opcioanl).")
			("reparar,r", po::value<string>(), "Archivo o Directorio a reparar.")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.size() == 0 || vm.count("ayuda")) cout << desc << endl;

		string fdb, directorio, archivo, romslocales;

		if (!vm.count("bdd")) throw string("Falta especificar la base de datos.");
		else fdb = vm["bdd"].as<string>();

		if (vm.count("local")) romslocales = vm["local"].as<string>();

		if (vm.count("reparar")){
			fs::path ruta(vm["reparar"].as<string>());

			if (fs::is_regular_file(ruta))
				reparar_archivo (fdb, vm["reparar"].as<string>(), romslocales);

			if (fs::is_directory(ruta))
				reparar_directorio (fdb, vm["reparar"].as<string>(), romslocales);

			return 0;
		}

		cout << "Que desea hacer?" << endl;

	}catch (string& e) {
		cerr << e << endl;
	}
}

void reparar_archivo (const string& bdd, const string& archivo, const string& romslocales)
{
	ModeloArchivosMame 	zips;

	try{
		vector<rom> rom_archivo;
		zips.contenido_zip(archivo, rom_archivo);
		path ruta(archivo);

		string juego_local(ierase_last_copy(to_lower_copy(ruta.filename()), ".zip"));

		VerificaRoms cr(bdd, romslocales);
		vector<pair<rom, VerificaRoms::estado> > resultado;

		if (cr.verificar(juego_local, rom_archivo, resultado) == 0)
			cout << juego_local << ": Correcto." << endl;
		else {
			cout << juego_local << ": INCorrecto." << endl;
			for (vector<pair<rom, VerificaRoms::estado> >::iterator itr=resultado.begin();itr!=resultado.end();++itr){
				switch (itr->second.estadoRom()) {
				case VerificaRoms::igual:
					cout << "\t" << itr->first.nombre << ": Correcta." << endl;
					break;
				case VerificaRoms::falta:
					cout << "\t" << itr->first.nombre << ": Falta" << endl;
					break;
				case VerificaRoms::distinto_nombre:
					cout << "\t" << itr->first.nombre << ": Tiene el nombre " << itr->second.nombre() << endl;
					break;
				case VerificaRoms::otro_archivo:
					cout << "\t" << itr->first.nombre << ": Esta en el archivo " << itr->second.archivo() << endl;
					break;
				case VerificaRoms::otro_archivo_nombre:
					cout << "\t" << itr->first.nombre << ": Esta en el archivo " << itr->second.archivo() << " con el nombre " << itr->second.nombre() << endl;
					break;
				default:
					cout << "\t" << itr->first.nombre << ": Desconocida?" << endl;
				}
			}
			cout << "Reparando ..." << endl;
			reparar_zip(archivo, resultado);
		}
	}catch (std::exception& e){
		cout << e.what() << endl;
	}
}

void reparar_directorio (const string& bdd, const string& directorio, const string& romslocales)
{
	ModeloArchivosMame 	zips;

	try{
		for (ModeloArchivosMame::iterator it=zips.begin(directorio);it!=zips.end();++it)
			reparar_archivo (bdd, directorio + it.nombre(), romslocales);
	}catch (std::exception& e){
		cout << e.what() << endl;
	}
}

void reparar_zip(const string& archivo, vector<pair<rom, VerificaRoms::estado> >& reparacion)
{
	string		temp;
	temp="/tmp/ArMame2/" + fs::path(archivo).filename() + "/";

	try	{
		fs::remove_all(temp);
	}catch(...) {cout << "OK" << endl;}

	GuruUnzip 	unzip(archivo);
	unzip.extraerTodo(temp);

	for (vector<pair<rom, VerificaRoms::estado> >::iterator itr=reparacion.begin();itr!=reparacion.end();++itr){
		switch (itr->second.estadoRom()) {
		case VerificaRoms::igual: break;
		case VerificaRoms::falta: break;
		case VerificaRoms::distinto_nombre:
			fs::rename(fs::path(temp + itr->second.nombre()), fs::path(temp + itr->first.nombre));
			break;
		case VerificaRoms::otro_archivo:{
			GuruUnzip unzip2(itr->second.archivo());
			unzip2.extraer(itr->first.nombre, temp);
			}
			break;
		case VerificaRoms::otro_archivo_nombre:{
			GuruUnzip unzip2(itr->second.archivo());
			unzip2.extraer(itr->second.nombre(), temp);
			fs::rename(fs::path(temp + itr->second.nombre()), fs::path(temp + itr->first.nombre));
			// cout << "\t" << itr->first.nombre << ": Esta en el archivo " << itr->second.archivo() << " con el nombre " << itr->second.nombre() << endl;
			}
			break;
		default:
			cout << "\t" << itr->first.nombre << ": Desconocida?" << endl;
		}
	}

	GuruZip		zip(archivo);
	fs::directory_iterator itfinal;

	for (fs::directory_iterator it(temp);it!=itfinal;++it){
		if (fs::is_regular_file(it->path())) {
			zip.agregar_archivo(it->path().filename(), temp + it->path().filename());
		}
	}

	zip.grabar();

	fs::remove_all(temp);
}
