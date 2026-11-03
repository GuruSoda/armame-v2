// #define _CRT_SECURE_NO_DEPRECATE _USE_32BIT_TIME_T

// #include <GuruUnzip.hpp>

#include <boost/program_options.hpp>

#include <map>
#include <iostream>

#include <ModeloMame.h>
#include <ModeloLocalRoms.h>
#include <GuruDirectorio.hpp>

using namespace boost::program_options;
using namespace std;

namespace po = boost::program_options;

void agregar_directorio (const string& fdb, const string& directorio);
void baseVSdirectorio(const string& fdb, const string& directorio);
void directorioVSbase(const string& fdb, const string& directorio);

int main(int argc, char* argv[])
{
/*
	map<string, unz_file_info> data;
	GuruUnzip::mapZip("d:\\Proyectos\\Desarrollo\\Librerias.zip", data);


	GuruUnzip::iterator itfinal;

	for (GuruUnzip::iterator it("d:\\Proyectos\\Desarrollo\\Librerias.zip");it!=itfinal;++it) {
		GuruBuffer b;
		GuruUnzip::cargar(it, b);
		cout << it.nombreArchivoActual() << " - " << it->uncompressed_size << endl;
	}
*/

	try{
		po::options_description desc("Parametros Validos");
		desc.add_options()
			("ayuda,h", "Esta pantalla.")
			("directorio,d", po::value<string>(), "Directorio a revisar.")
			("bdd,b", po::value<string>(), "Base de Datos a guardar la revision.")
			("crear,c", "Crea la base de datos vacia.")
			("scan,s", "Verifica los archivos y actualiza cambios.")
			("agregar,a", "Agrega un nuevo directorio a la base de datos.")
			;

		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.size() == 0 || vm.count("ayuda")) cout << desc << endl;

		string fdb;
		string directorio;

		if (!vm.count("bdd")) throw string("Falta especificar la base de datos.");
		else fdb = vm["bdd"].as<string>();

		if (vm.count("crear")){
			ModeloLocalRoms(vm["bdd"].as<string>()).crear_base();
			return 0;
		}

		if (!vm.count("directorio")) throw string("Falta especificar el directorio.");
		else directorio = vm["directorio"].as<string>();

		if (vm.count("scan")){
			baseVSdirectorio(fdb, directorio);
			directorioVSbase(fdb, directorio);
			return 0;
		}

		if (vm.count("agregar")){
			agregar_directorio (fdb, directorio);
			return 0;
		}

		cout << "Que desea hacer?" << endl;

	}catch (string& e) {
		cerr << e << endl;
	}

	return 0;
}

void agregar_directorio (const string& fdb, const string& directorio)
{
	try{
		if (directorio.empty()) throw string("El directorio no contiene archivos.");

		ModeloLocalRoms mimame(fdb);
		GuruDirectorio::iterator_recursivo itfinal;

		time_t inicio,fin;
		time(&inicio);

		for (GuruDirectorio::iterator_recursivo it(directorio,"^.*zip$");it!=itfinal;++it) {
			cout << "Catalogando:" << it.dir() << it->sacar_nombre();
			try{
				mimame.catalogarArchivo(it->sacar_nombre(), it.dir());
			}catch (string e) {
				cerr << it->sacar_nombre() << " --> Ya existe en el catalogo." << endl;
				continue;
			}
			cout << endl;
		}

		time(&fin);
		cout << "Segundos transcurridos:" << (fin-inicio) << endl;

/*
		ModeloArchivosMame archizip;

		time_t inicio,fin;
		time(&inicio);
		for (ModeloArchivosMame::iterator it=archizip.begin(directorio);it!=archizip.end();++it) {
			cout << "Catalogando:" << it.nombre();
			try{
				mimame.catalogarArchivo(it.nombre(), directorio);
				cout << " Bien!" << endl;
			}catch (string e) {cerr << it.nombre() << " --> Ya existe en el catalogo." << endl;}
		}
		time(&fin);
		cout << "Segundos transcurridos:" << (fin-inicio) << endl;
*/
	} catch(const string& e) {cout << "Error: " << e << endl;}
}

void baseVSdirectorio(const string& fdb, const string& directorio)
{
	try{
		ModeloLocalRoms lasroms(fdb);

		list<propiedadesArchivoZip> listado;
		lasroms.archivos(directorio, listado);

		if (listado.empty()) throw string("NO existe el directorio o no contiene archivos.");

		for (list<propiedadesArchivoZip>::iterator it=listado.begin();it!=listado.end();++it){
			GuruDirectorio::GuruArchivo archi;

			GuruDirectorio::propiedades_archivo(directorio + it->nombre(), archi);

			if (!archi.Existe()) {
				cout << "Borrando:" << it->nombre() << endl;
				lasroms.borrar_archivo(it->nombre(), directorio);
				continue;
			}

			if ((archi.sacar_tamanio() != it->bytes()) || (archi.sacar_fecha() != it->fecha())) {

//				if (archi.sacar_tamanio() != it->bytes())
//					cout << it->nombre() << ": Difiere el tamaño (" << archi.sacar_tamanio() << "<->" << it->bytes() << ")"<< endl;

//				if (archi.sacar_fecha() != it->fecha())
//					cout << it->nombre() << ": Difiere la fecha, Local=" << it->string_fecha<string>(archi.sacar_fecha()) << " | Base=" << it->string_fecha<string>(it->fecha()) << endl;

				cout << "Actualizando:" << it->nombre() << endl;

				lasroms.borrar_archivo(it->nombre(), directorio);

				lasroms.catalogarArchivo(archi.sacar_nombre(), directorio);
			}
		} // for

	} catch(const string&) { throw; }
}

void directorioVSbase(const string& fdb, const string& directorio)
{
	try{
		ModeloLocalRoms lasroms(fdb);
		GuruDirectorio::iterator_recursivo itfinal;

		for (GuruDirectorio::iterator_recursivo it(directorio);it!=itfinal;++it){
			if (!lasroms.existe_archivo(it->sacar_nombre(), it->sacar_tamanio(), it->sacar_fecha())){
				cout << "Agregando:" << it->sacar_nombre() << endl;
				lasroms.catalogarArchivo(it->sacar_nombre(), directorio);
			}
		}
	} catch(const string&) { throw; }
}

/*
	GuruUnzip unzip;

	map<string, unz_file_info> archivos;

	unzip.contenido("C:\\duke3dsource.zip", archivos);

	GuruBuffer buff;

	try{
		unzip.extraer("C:\\duke3dsource.zip", "AudioLib.zip", buff);

		cout << GuruSHA1::sha1((byte*)buff.contenido(), buff.longitud_buffer()) << endl;

//		unzip.extraer("C:\\duke3dsource.zip", "AudioLib.zip", "D:\\");
	}catch(const char* e) {cout << e << endl;}

	unz_file_info data;
	for (map<string, unz_file_info>::iterator it=archivos.begin();it!=archivos.end();++it)
		{
		cout << it->first << endl;
		data = it->second;

		cout << it->second.tmu_date.tm_hour << ":" << it->second.tmu_date.tm_min << ":" << it->second.tmu_date.tm_sec << endl;
		cout << it->second.tmu_date.tm_year << "/" << it->second.tmu_date.tm_mon << "/" << it->second.tmu_date.tm_mday << endl;

		cout << archivos["GNU.TXT"].dosDate << endl;
		}
*/
