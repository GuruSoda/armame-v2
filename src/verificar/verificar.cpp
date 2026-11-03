#include <boost/program_options.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/filesystem.hpp>
#include <ModeloArchivosMame.h>
#include <GuruMameUtil.h>
#include <GuruUnzip.h>

using namespace boost::program_options;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace std;

namespace po = boost::program_options;

void verificar_archivo (const string& bdd, const string& archivo, const string& romlocales);
void verificar_directorio (const string& bdd, const string& directorio, const string& romlocales);

int main (int argc, char* argv[])
{
	try{
		po::options_description desc("Parametros Validos");
		desc.add_options()
			("ayuda,h", "Esta pantalla.")
			("help,h", "Esta pantalla.")
			("bdd,b", po::value<string>(), "Base de Datos del M.A.M.E..")
			("local,l", po::value<string>(), "Base de Datos de Roms Locales (Optativo).")
			("verificar,v", po::value<string>(), "Archivo o Directorio a verificar.")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.size() == 0 || vm.count("ayuda")) cout << desc << endl;

		string fdb, directorio, archivo, romslocales;

		if (!vm.count("bdd")) throw string("Falta especificar la base de datos.");
		else fdb = vm["bdd"].as<string>();

		if (vm.count("local")) romslocales = vm["local"].as<string>();

		if (vm.count("verificar")){
			fs::path ruta(vm["verificar"].as<string>());

			if (fs::is_regular_file(ruta))
				verificar_archivo (fdb, vm["verificar"].as<string>(), romslocales);

			if (fs::is_directory(ruta))
				verificar_directorio (fdb, vm["verificar"].as<string>(), romslocales);

			return 0;
		}

		cout << "Que desea hacer?" << endl;

	}catch (string& e) {
		cerr << e << endl;
	}
}

void verificar_archivo (const string& bdd, const string& archivo, const string& romslocales)
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
				switch (itr->second.estadoRom())
					{
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
		}
	}catch (std::exception& e){
		cout << e.what() << endl;
	}
}

void verificar_directorio (const string& bdd, const string& directorio, const string& romslocales)
{
	ModeloArchivosMame 	zips;

	try{
		for (ModeloArchivosMame::iterator it=zips.begin(directorio);it!=zips.end();++it)
			verificar_archivo (bdd, directorio + it.nombre(), romslocales);
	}catch (std::exception& e){
		cout << e.what() << endl;
	}
}
