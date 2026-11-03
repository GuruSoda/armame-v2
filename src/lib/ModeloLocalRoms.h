#ifndef __MODELOLOCALROMS_H__
#define __MODELOLOCALROMS_H__

#include <GuruCompat.h>
#include <GuruFDBBase.h>
#include <GuruSHA1.h>
#include <GuruCRC.h>
#include <GuruDirectorio.hpp>
#include <GuruUnzip.hpp>
#include <GuruID.hpp>

#include <GuruMameListInfo.h>

#include <list>
#include <map>

#include <boost/algorithm/string/case_conv.hpp>

using namespace boost::algorithm;
using namespace std;


class propiedadesArchivoZip
{
public:
	propiedadesArchivoZip() {};
	virtual ~propiedadesArchivoZip() {};

	void tomarPropiedades(const string& archivo) {
		GuruDirectorio::GuruArchivo ruta = GuruDirectorio::propiedades_archivo(archivo);

		clear();

		nombre(ruta.sacar_nombre());
		bytes(ruta.sacar_tamanio());
		fecha(ruta.sacar_fecha());
		crc(GuruSHA1::archivo_sha1(archivo));
	}

	inline string nombre() {return _nombre;};
	inline void nombre(const string& n) {_nombre=n;};

	inline time_t fecha() {return _fecha;};
	inline void fecha(time_t f) {_fecha=f;};

	inline string crc() {return _crc;};
	inline void crc(const string& c) {_crc=c;};

	inline ::uint64_t bytes() {return _bytes;};
	inline void bytes (::uint64_t b) {_bytes=b;};

	template <class T>
	T string_fecha(time_t lafecha) {
		struct tm* timeinfo = {0};
		timeinfo = localtime(&lafecha);

		stringstream buff;

		buff << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec << " - " << timeinfo->tm_mday << "/" << timeinfo->tm_mon + 1 << "/" << timeinfo->tm_year + 1900;

		return T(buff.str());
	}

	void clear() {
		_nombre.clear();
		_fecha = 0;
		_crc.clear();
		_bytes = 0;
	};

	friend class ModeloLocalRoms;

private:
	string _nombre;
	time_t _fecha;
	string _crc;
	::uint64_t _bytes;
};

class ModeloLocalRoms: public GuruFDBBase
{
public:
	ModeloLocalRoms(const string& archivo_fdb):GuruFDBBase(archivo_fdb) {
		#if defined IBPP_WINDOWS
			IBPP::ClientLibSearchPaths("..\\win32");
		#endif
	} ;

	virtual ~ModeloLocalRoms(void) {};

	void crear_base();

	map<string,vector<rom> >& roms2map(map<string,vector<rom> >& catalogo);

	void		borrar_archivo(const string& nombre, const string& directorio);
	void		catalogarArchivo(const string& nombre, const string& directorio = "");

	bool		existe_archivo(const string& nombre, uint64_t bytes = 0, time_t fecha = 0, const string& crc = "");
	list<propiedadesArchivoZip>& archivos(const string& directorio, list<propiedadesArchivoZip>& listado);

	void 		sacar_roms(const string& nombre_archivo, const string& directorio, vector<rom>& contenido);
	pair<string, string> buscar_rom(const string& nombre_zip, const string& nombre, uint64_t tamanio, const string& crc, const string& sha1);
	bool 		verificar_juego(const vector<rom>& roms_mame);

private:
	int32_t id_directorio(const string& directorio);
	int32_t id_archivo(const string& nombre_archivo, const string& directorio);
	int32_t id_rom(const string& nombre_rom, const string& directorio);

	int32_t	insertar_directorio(const string& directorio);
	int32_t	insertar_archivo(const string& archivo, const string& directorio);
	int32_t	insertar_rom(const string& archivo, const rom& mirom, const string& directorio);

	map<string, int32_t> cacheArchivos;
	map<string, int32_t> cacheDirectorios;

	GuruID<string>		idArchivos;
	GuruID<string>		idDirectorios;
};

#endif
