#ifndef __VERIFICAJUEGO_H__
#define __VERIFICAJUEGO_H__

#include <ModeloMame.h>
#include <ModeloLocalRoms.h>
#include <GuruMameUtil.h>

class VerificaJuego {
public:
	enum TipoBusqueda {bdd=0, memoria};
	enum Estado {
		igual=0,				// Las 2 roms son iguales.
		distinto_nombre,		// Tiene distinto nombre pero sus hash's y tamanio son iguales.
		distinto_hash,			// Tiene el mismo nombre pero sus hash's y tamanio son distintos.
		falta,					// No existe la rom.
		otro_archivo,			// La rom le pertenece a otro juego.
		otro_archivo_nombre};	// La rom le pertenece a otro juego y con otro nombre.

	class EstadoRom {
	public:
		EstadoRom(const string& nombre_rom, const string& nombre_archivo, enum Estado estado_comparacion):
				_estado_rom(estado_comparacion), _original(nombre_rom), _archivo(nombre_archivo){};

		string& nombre() {return _original;};
		string& archivo() {return _archivo;};
		Estado estadoRom() {return _estado_rom;};

	private:
		Estado _estado_rom;
		string _original;
		string _archivo;
	};

public:
	VerificaJuego(ModeloMame& m_mame, ModeloLocalRoms& m_localroms, TipoBusqueda busqueda=VerificaJuego::bdd):_mame(m_mame),_localroms(m_localroms),tipoBusqueda(busqueda) {};
	virtual ~VerificaJuego() {};

	int mameVSlocalroms(const string& juego, vector<rom>& mis_roms, vector<pair<rom, EstadoRom> >& resultado);
	int mameVSlocalroms(const string& juego, vector<pair<rom, EstadoRom> >& resultado);
	void localromsVSmame();

private:
	ModeloMame& 		_mame;
	ModeloLocalRoms& 	_localroms;
	TipoBusqueda		tipoBusqueda;

	map<string,vector<rom> > catalogomame;
	map<string,vector<rom> > catalogolocalroms;
};

#endif
