#include "ModeloMame.h"

void ModeloMame::crear_modelo()
{
const char* ddl_modelo_mame[] = {"\
	CREATE TABLE ROM ( \
		id_rom  INTEGER NOT NULL, \
		nombre  VARCHAR(45) NOT NULL, \
		tamanio INTEGER NOT NULL, \
		crc     VARCHAR(10) NOT NULL, \
		sha1    VARCHAR(50) NOT NULL, \
	\
		UNIQUE(nombre,tamanio,crc,sha1), \
		PRIMARY KEY(id_rom) \
	)\
	","\
	CREATE TABLE MANUFACTURER ( \
		id_manufacturer INTEGER NOT NULL, \
		descripcion VARCHAR(200) NOT NULL, \
	\
		UNIQUE (descripcion), \
		PRIMARY KEY (id_manufacturer) \
	)\
	","\
	CREATE TABLE GAME ( \
		id_game INTEGER NOT NULL, \
		nombre  VARCHAR(64) NOT NULL, \
		descripcion VARCHAR(200) NOT NULL, \
		id_clonof INTEGER, \
		id_romof  INTEGER, \
		anio SMALLINT, \
		id_manufacturer SMALLINT, \
	\
		UNIQUE (nombre), \
		PRIMARY KEY (id_game), \
		FOREIGN KEY(id_clonof) REFERENCES GAME(id_game) on delete cascade, \
		FOREIGN KEY(id_manufacturer) REFERENCES MANUFACTURER(id_manufacturer) on delete cascade \
	)\
	","\
	CREATE TABLE ROM_GAME( \
		id_game     INTEGER NOT NULL, \
		id_rom      INTEGER NOT NULL, \
		combinado   CHAR(1) DEFAULT 'N', \
	\
		FOREIGN KEY(id_game) REFERENCES GAME(id_game) on delete cascade, \
		FOREIGN KEY(id_rom) REFERENCES ROM(id_rom) on delete cascade \
	)\
	","\
	CREATE GENERATOR GEN_ID_ROM; \
	","\
	CREATE GENERATOR GEN_ID_MANUFACTURER;\
	","\
	CREATE GENERATOR GEN_ID_GAME; \
	","\
	CREATE TRIGGER BI_ROM FOR ROM \
	ACTIVE BEFORE INSERT \
	POSITION 0 \
	AS \
	BEGIN \
		IF (NEW.ID_ROM IS NULL) THEN \
			NEW.ID_ROM = GEN_ID(GEN_ID_ROM, 1); \
	END\
	","\
	CREATE TRIGGER BI_MANUFACTURER FOR MANUFACTURER \
	ACTIVE BEFORE INSERT \
	POSITION 0 \
	AS \
	BEGIN \
		IF (NEW.ID_MANUFACTURER IS NULL) THEN \
			NEW.ID_MANUFACTURER = GEN_ID(GEN_ID_MANUFACTURER, 1); \
	END\
	","\
	CREATE TRIGGER BI_GAME FOR GAME \
	ACTIVE BEFORE INSERT \
	POSITION 0 \
	AS \
	BEGIN \
		IF (NEW.ID_GAME IS NULL) THEN \
			NEW.ID_GAME = GEN_ID(GEN_ID_GAME, 1); \
	END\
	",NULL};

	crear_bdd(ddl_modelo_mame);
}

int32_t ModeloMame::insertar_manufacturer(const string& manufacturer)
{
	int32_t id = 0;

	try{
		map<string, int32_t>::iterator it;
		if ((it=cacheManufacturer.find(manufacturer)) != cacheManufacturer.end()){
			return it->second;
		}

		__INIT_STATEMENT_START__

		st->Prepare("insert into manufacturer(descripcion) values (?) returning id_manufacturer;");
		st->Set(1, manufacturer);
	
		st->Execute();

		st->Get(1, id);

		st->TransactionPtr()->Commit();

		cacheManufacturer.insert(make_pair(manufacturer, id));
	}catch (IBPP::SQLException&) {
		throw ;
	}

	return id;
}

int32_t ModeloMame::insertar_game(const string& nombre, const string& descripcion, const string& clon_de, const string& rom_de, const string& anio, const string& fabricante)
{
	int32_t id = 0;

	try {
		map<string, int32_t>::iterator it;
		if ((it=cacheGame.find(nombre)) != cacheGame.end()){
			return it->second;
		}

		__INIT_STATEMENT_START__

		st->Prepare("insert into game(nombre, descripcion, id_clonof, id_romof, anio, id_manufacturer) values (?,?,?,?,?,?) returning id_game;");
		st->Set(1, nombre);
		st->Set(2, descripcion);
		st->SetNull(3);
		if (!clon_de.empty())
			if ((id=id_game(clon_de)) != 0)
				st->Set(3, id);
		st->SetNull(4);
		if (!rom_de.empty())
			if ((id=id_game(rom_de)) != 0)
				st->Set(4, id);
		st->Set(5, static_cast<int32_t>(atol(anio.c_str())));
		st->Set(6, insertar_manufacturer(fabricante));

		try{
			st->Execute();

			st->Get(1, id);

			st->TransactionPtr()->Commit();

			cacheGame.insert(make_pair(nombre, id));
		}catch (IBPP::Exception&) {
			st->TransactionPtr()->Rollback();
		//	cout << e.what() << endl;
		}
	}catch (IBPP::SQLException& e) {
		cout << e.what() << endl;
	}

	return id;
}

int32_t ModeloMame::insertar_rom(const rom& larom)
{
	int32_t id=0;

	try{

		map<rom, int32_t>::iterator itc;
		if ((itc=cacheRom.find(larom)) != cacheRom.end())
			return itc->second;

		__INIT_STATEMENT_START__

		st->Prepare("insert into rom(nombre, tamanio, crc, sha1) values (?,?,?,?) returning id_rom;");
		st->Set(1, larom.nombre);
		st->Set(2, (int32_t)larom.tamanio);
		st->Set(3, larom.crc.empty() ? "00000000" : larom.crc);
		st->Set(4, larom.sha1.empty() ? "0000000000000000000000000000000000000000" : larom.sha1);

		try{
			st->Execute();

			st->Get(1, id);

			st->TransactionPtr()->Commit();

			cacheRom.insert(make_pair(larom, id));
		}catch (IBPP::Exception&)	{
			st->TransactionPtr()->Rollback();

			id = id_rom(larom.nombre, larom.tamanio, larom.crc, larom.sha1);
		}
	}catch (IBPP::SQLException& e) {cout << e.what() << endl;}

	return id;
}

void ModeloMame::insertar_roms(const string& nombre, const vector<rom>& lasroms)
{
	for (vector<rom>::const_iterator it=lasroms.begin();it!=lasroms.end();++it)
			insertar_rom(*it);
}

void ModeloMame::insertar_rom_game(int32_t idrom, int32_t idgame, bool merge)
{
	__INIT_STATEMENT_START__

	try{
		st->Prepare("insert into rom_game(id_game, id_rom, combinado) values (?,?,?);");
		st->Set(1, idgame);
		st->Set(2, idrom);
		st->Set(3, merge ? "S" : "N");

		st->Execute();

		st->TransactionPtr()->Commit();
	}catch (IBPP::Exception& e) {
		cout << e.what() << endl;
	}
}

void ModeloMame::insertar_juego(juego& eljuego)
{
	try{
		int32_t _id_game = insertar_game(eljuego.nombre, eljuego.descripcion, eljuego.clon_de, eljuego.rom_de, eljuego.anio, eljuego.fabricante);

		for (vector<rom>::iterator it=eljuego.lista_roms.begin();it!=eljuego.lista_roms.end();++it){
			int32_t _id_rom = insertar_rom(*it);

			insertar_rom_game(_id_rom, _id_game, it->unido.empty()?false:true);
		}

	} catch (string& e) {cout << e << endl;}
}

int32_t ModeloMame::id_game(const string& game)
{
	int32_t id = 0;
	try{
		__INIT_STATEMENT_START__

		st->Prepare("select id_game from game where lower(nombre) = lower(?);");
		st->Set(1, game);
		st->Execute();

		if (st->Fetch()) st->Get(1, id);

		tr->Commit();
	}catch (IBPP::Exception& e)	{
		std::cerr << e.what();
	};

	return id;
}

int32_t ModeloMame::id_rom(const string& nombre, long tamanio, const string& crc, const string& sha1)
{
	int32_t id = 0;

	try{
		__INIT_STATEMENT_START__	
		
		st->Prepare("select id_rom from rom where nombre=? and tamanio = ? and crc=? and sha1=?;");
		st->Set(1, nombre);
		st->Set(2, (int32_t)tamanio);
		st->Set(3, crc);
		st->Set(4, sha1);

		st->Execute();

		if (st->Fetch()) st->Get(1, id);

		tr->Commit();
	}catch (IBPP::Exception& e)	{
		std::cerr << e.what();
	};

	return id;
}

vector<rom> ModeloMame::sacar_roms(const string& nombre_juego, bool con_combinados)
{
	rom data_rom;
	vector<rom> lista_roms;
	try{
		__INIT_STATEMENT_START__		
			
		st->Prepare("SELECT lower(B.nombre), B.tamanio, lower(B.crc), lower(B.sha1) FROM GAME A, ROM B, ROM_GAME C \
WHERE C.id_game = A.ID_GAME AND C.id_rom = B.id_rom AND lower(A.NOMBRE) = lower(?) AND (combinado = ? OR combinado = 'N');");
		st->Set(1, nombre_juego);
		st->Set(2, con_combinados ? "S" : "N");
		st->Execute();

		while (st->Fetch()) {
			st->Get(1, data_rom.nombre);
			st->Get(2, (int32_t*)&data_rom.tamanio);
			st->Get(3, data_rom.crc);
			st->Get(4, data_rom.sha1);

			lista_roms.push_back(data_rom);
		}

		st->TransactionPtr()->Commit();
	}catch (IBPP::Exception& e)	{
		std::cerr << e.what();
	};

	return vector<rom>(lista_roms);
}

vector<string> ModeloMame::sacar_games()
{
	string game;
	vector<string> lista_games;

	try{
		__INIT_STATEMENT_START__

		st->Execute("select nombre from game order by nombre;");

		while (st->Fetch()) {
			st->Get(1, game);

			lista_games.push_back(game);
		}

		st->TransactionPtr()->Commit();
	}catch (IBPP::Exception& e)	{
		std::cerr << e.what();
	};

	return vector<string>(lista_games);
}

map<string,vector<rom> >& ModeloMame::roms2map(map<string,vector<rom> >& catalogo)
{
	try{
		__INIT_STATEMENT_START__

		st->Execute("select GAME.NOMBRE,ROM.NOMBRE,ROM.TAMANIO,ROM.CRC,ROM.SHA1 from GAME,ROM,ROM_GAME \
where ROM_GAME.ID_GAME = GAME.ID_GAME and ROM_GAME.ID_ROM = ROM.ID_ROM and rom_game.COMBINADO='N'");

		string nombre_juego;
		string nombre_anterior;
		vector<rom> lista_roms;

		while (st->Fetch()) {
			rom data_rom;

			st->Get(1, nombre_juego);
			if (nombre_anterior.empty()) nombre_anterior = nombre_juego;
			st->Get(2, data_rom.nombre);
			st->Get(3, (int32_t*)&data_rom.tamanio);
			st->Get(4, data_rom.crc);
			st->Get(5, data_rom.sha1);

			if (nombre_anterior == nombre_juego)
				lista_roms.push_back(data_rom);
			else {
				catalogo.insert(make_pair(nombre_anterior, lista_roms));
				nombre_anterior = nombre_juego;
				lista_roms.clear();
				lista_roms.push_back(data_rom);
			}
		}

		st->TransactionPtr()->Commit();
	}catch (IBPP::Exception& e)	{
		std::cerr << e.what();
	};

	return catalogo;
}

map<rom, long>& ModeloMame::roms2map(map<rom, long>& mapa_id_rom)
{
	__INIT_STATEMENT__

	try{
		st->TransactionPtr()->Start();
		st->Execute("select id_rom,nombre,tamanio,crc,sha1 from rom order by nombre");

		while (st->Fetch()) {
			long id_rom = 0;
			rom data_rom;

			st->Get(1, (int32_t*)&id_rom);
			st->Get(2, data_rom.nombre);
			st->Get(3, (int32_t*)&data_rom.tamanio);
			st->Get(4, data_rom.crc);
			st->Get(5, data_rom.sha1);

			mapa_id_rom.insert(make_pair(data_rom, id_rom));
		}

		st->TransactionPtr()->Commit();
	}catch (IBPP::Exception& e)	{
		st->TransactionPtr()->Rollback();
		std::cerr << e.what();
	};

	return mapa_id_rom;
}

map<string, long>& ModeloMame::games2map(map<string, long>& mapa_id_game)
{
	__INIT_STATEMENT__

	try{
		st->TransactionPtr()->Start();
		st->Execute("select id_game,nombre from game");

		while (st->Fetch()) {
			long id_game = 0;
			string nombre_game;

			st->Get(1, (int32_t*)&id_game);
			st->Get(2, nombre_game);

			mapa_id_game.insert(make_pair(nombre_game, id_game));
		}

		st->TransactionPtr()->Commit();
	}catch (IBPP::Exception& e)	{
		st->TransactionPtr()->Rollback();
		std::cerr << e.what();
	};

	return mapa_id_game;
}
