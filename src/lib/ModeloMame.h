#ifndef __MODELOMAME_H__
#define __MODELOMAME_H__

#include <string>
#include <iostream>
#include <map>
#include <boost/algorithm/string.hpp>

#include <GuruCompat.h>
#include <GuruMameListInfo.h>
#include <GuruFDBBase.h>

using namespace std;

class ModeloMame: public GuruFDBBase
{
public:
	ModeloMame(const string& archivo_fdb):GuruFDBBase(archivo_fdb) {
		#if defined IBPP_WINDOWS
			IBPP::ClientLibSearchPaths("..\\win32");
		#endif
	} ;

	~ModeloMame(void) {};

	void crear_modelo();
	int32_t insertar_manufacturer(const string& manufacturer);
	int32_t insertar_game(const string& nombre, const string& descripcion, const string& clon_de, const string& rom_de, const string& anio, const string& fabricante);
	int32_t insertar_rom(const rom& larom);
	void insertar_rom_game(int32_t idrom, int32_t idgame, bool merge);

	void insertar_roms(const string& nombre, const vector<rom>& lasroms);

	int32_t id_game(const string& game);
	int32_t id_rom(const string& nombre, long tamanio, const string& crc="00000000", const string& sha1="0000000000000000000000000000000000000000");

	vector<rom> 	sacar_roms(const string& nombre_juego, bool con_combinados = false);
	vector<string> 	sacar_games();

	map<string,vector<rom> >& roms2map(map<string,vector<rom> >& catalogo);
	map<rom, long>& 		roms2map(map<rom, long>& mapa_id_rom);
	map<string, long>& 		games2map(map<string, long>& mapa_id_game);

	void 	vincular_game_roms();

	void	verificar_games();

	void	insertar_juego(juego& eljuego);

private:
	map<string, int32_t> cacheManufacturer;
	map<string, int32_t> cacheGame;
	map<rom, int32_t> cacheRom;
};

#endif
