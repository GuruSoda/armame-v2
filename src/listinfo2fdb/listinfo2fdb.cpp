#include <boost/program_options.hpp>
#include <string>
#include <iostream>
#include <list>
#include <map>
#include <time.h>

#include <GuruMameListInfo.h>
#include <ModeloMame.h>

namespace po = boost::program_options;

using namespace std;
using namespace boost;

void cargarBase(string& listinfo, string& fdb);

int main(int argc, char* argv[])
{
	string listinfo;
	string fdb;

	try {
		po::options_description desc("Parametros Validos");
		desc.add_options()
			("ayuda,h", "Esta pantalla.")
			("listinfo,l", po::value<string>(), "Archivo ListInfo.")
			("bdd,b", po::value<string>(), "Base de Datos a guardar ListInfo.")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.size() == 0 || vm.count("ayuda")) cout << desc << endl;

		if (!vm.count("bdd")) throw string("Falta especificar el nombre de la base de datos.");
		else fdb = vm["bdd"].as<string>();

		if (!vm.count("listinfo")) throw string("Falta especificar el archivo listinfo.");
		else listinfo = vm["listinfo"].as<string>();

		cargarBase(listinfo, fdb);

	}catch(const string& e) {cout << e << endl;}
	 catch( ... ) {cout << "Error inesperado e inexplicable." << endl;};

	return 0;
}

void cargarBase(string& nombre_listinfo, string& nombre_fdb)
{
	GuruMameListInfo listinfo(nombre_listinfo, false);
	ModeloMame mame(nombre_fdb);

	mame.crear_modelo();

	listinfo.abrir();

	unsigned long i=1;
	for (GuruMameListInfo::iterator it=listinfo.begin();it!=listinfo.end();++it,++i) {
		cout << "Insertando: [" << i << "] " << it->nombre << " - " << it->descripcion << endl;
		mame.insertar_juego(*it);
	}

	listinfo.cerrar();
}
