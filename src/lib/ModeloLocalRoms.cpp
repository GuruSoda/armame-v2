#include "ModeloLocalRoms.h"

void ModeloLocalRoms::crear_base()
{
	const char* ddl_modelolocalroms[] = {"\
CREATE TABLE DIRECTORIO (\
	id_directorio integer not null, \
	directorio		varchar (268) not null, \
\
	unique(directorio), \
	primary key (id_directorio)\
);\
","\
CREATE TABLE ARCHIVO ( \
    id_archivo  INTEGER NOT NULL, \
    nombre  	VARCHAR(64) NOT NULL, \
	fecha 		TIMESTAMP NOT NULL, \
	tamanio		INTEGER NOT NULL, \
	sha1		VARCHAR(50) NOT NULL, \
    id_directorio  integer NOT NULL, \
	estado		smallint default 0, \
\
	UNIQUE(nombre,id_directorio), \
    PRIMARY KEY(id_archivo), \
	FOREIGN KEY(id_directorio) REFERENCES DIRECTORIO(id_directorio) on delete cascade \
);\
","\
CREATE TABLE ROM ( \
    id_rom		INTEGER NOT NULL, \
	id_archivo  INTEGER NOT NULL, \
    nombre		VARCHAR(64) NOT NULL, \
    tamanio		INTEGER NOT NULL, \
    crc			VARCHAR(10) NOT NULL, \
	sha1		VARCHAR(50) NOT NULL, \
\
	UNIQUE(id_archivo,nombre,tamanio,crc,sha1), \
	FOREIGN KEY(id_archivo) REFERENCES ARCHIVO(id_archivo) on delete cascade, \
    PRIMARY KEY(id_rom) \
);\
","\
CREATE SEQUENCE SEQ_ID_ROM; \
","\
CREATE SEQUENCE SEQ_ID_ARCHIVO; \
"," \
CREATE SEQUENCE SEQ_ID_DIRECTORIO;", NULL};

	crear_bdd(ddl_modelolocalroms);
}

int32_t ModeloLocalRoms::id_archivo(const string& archivo, const string& directorio)
{
	int32_t id = 0;

	try{
		__INIT_STATEMENT_START__

		st->Prepare("select id_archivo from archivo A, directorio B \
					where A.nombre = ? \
					and B.directorio = ? \
					and A.id_directorio = B.id_directorio;");
		st->Set(1, archivo);
		st->Set(2, directorio);
		st->Execute();

		if (st->Fetch()) 
			st->Get(1, id);

		tr->Commit();
	}catch (IBPP::Exception& e)	{
		std::cerr << e.what();
	};

	return id;
}

int32_t ModeloLocalRoms::id_directorio(const string& directorio)
{
	int32_t id = 0;

	try{
		__INIT_STATEMENT_START__

		st->Prepare("select id_directorio from directorio where directorio = ?;");
		st->Set(1, directorio);
		st->Execute();

		if (st->Fetch()) st->Get(1, id);

		tr->Commit();
	}catch (IBPP::Exception& e)	{
		std::cerr << e.what();
	};

	return id;
}

int32_t ModeloLocalRoms::insertar_directorio(const string& directorio)
{
	int32_t id = 0;

	try{
		map<string, int32_t>::iterator it;
		if ((it=cacheDirectorios.find(directorio)) != cacheDirectorios.end()){
			return it->second;
		}

		__INIT_STATEMENT_START__

		st->Prepare("insert into directorio(id_directorio, directorio) \
					values (next value for SEQ_ID_DIRECTORIO, ?) returning id_directorio;");
		st->Set(1, directorio);
		st->Execute();

		st->Get(1, id);

		tr->Commit();

		cacheDirectorios.insert(make_pair(directorio, id));
	}catch (IBPP::SQLException& e)	{
		if (e.SqlCode() != -803){ //Invalid insert or update value(s): object columns are constrained - no 2 table rows can have duplicate column values
			cerr << "Error: " << e.what() << endl;
			cerr << "Engine Code:" << e.EngineCode() << endl;
			cerr << "Sql Code:" << e.SqlCode() << endl;

			throw string(e.what());
		}else { id = id_directorio(directorio);}
	}

	return id;
}

int32_t ModeloLocalRoms::insertar_archivo(const string& nombre, const string& directorio)
{
	int32_t id = 0;

	try {

		map<string, int32_t>::iterator it;
		if ((it=cacheArchivos.find(directorio + nombre)) != cacheArchivos.end()){
			return it->second;
		}

		GuruDirectorio::GuruArchivo statzip;

		GuruDirectorio::propiedades_archivo(directorio + nombre, statzip);

		__INIT_STATEMENT_START__

		st->Prepare("insert into ARCHIVO(id_archivo, nombre, fecha, tamanio, sha1, id_directorio) \
					values (next value for SEQ_ID_ARCHIVO, ?, ?, ?, lower(?), ?) returning id_archivo;");
		st->Set(1, nombre);
		st->Set(3, static_cast<int32_t>(statzip.sacar_tamanio()));
		st->Set(4, GuruSHA1::archivo_sha1(directorio + nombre));
		// st->Set(4, "0000000000000000000000000000000000000000");
		st->Set(5, insertar_directorio(directorio));

		struct tm* timeinfo = {0};
		time_t timestamp=statzip.sacar_fecha();
		timeinfo = localtime(&timestamp);

		st->Set(2, IBPP::Timestamp(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec));

		try{
			st->Execute();

			st->Get(1, id);

			tr->Commit();

			// cout << "Insertando:" << nombre << ": " << timeinfo->tm_hour << ":" << timeinfo->tm_min << ":" << timeinfo->tm_sec << " - " << timeinfo->tm_mday << "/" << timeinfo->tm_mon + 1 << "/" << timeinfo->tm_year + 1900 << endl;

			cacheArchivos.insert(make_pair(directorio + nombre, id));
		}catch (IBPP::SQLException& e)	{
			tr->Rollback();
			if (e.SqlCode() != -803){ //Invalid insert or update value(s): object columns are constrained - no 2 table rows can have duplicate column values
				cerr << "Error: " << e.what() << endl;
				cerr << "Engine Code:" << e.EngineCode() << endl;
				cerr << "Sql Code:" << e.SqlCode() << endl;

				throw string(e.what());
			} else {id=id_archivo(nombre, directorio);}
		}
	} catch (IBPP::LogicException& e) {throw string(e.what());}

	return id;
}

int32_t ModeloLocalRoms::insertar_rom(const string& archivo, const rom& mirom, const string& directorio)
{
	int32_t id = 0;

	try {
		__INIT_STATEMENT_START__

		st->Prepare("insert into ROM(id_rom, id_archivo, nombre, tamanio, crc, sha1) \
					values (next value for SEQ_ID_ROM,?,?,?,lower(?),lower(?)) returning id_rom;");

		st->Set(1, insertar_archivo(archivo, directorio));
		st->Set(2, mirom.nombre);
		st->Set(3, static_cast<int32_t>(mirom.tamanio));
		st->Set(4, mirom.crc);
		st->Set(5, mirom.sha1);

		try{
			st->Execute();

			st->Get(1, id);

			tr->Commit();
		}catch (IBPP::SQLException& e)	{
			tr->Rollback();
			if (e.SqlCode() != -803){ //Invalid insert or update value(s): object columns are constrained - no 2 table rows can have duplicate column values
				cerr << "Error: " << e.what() << endl;
				cerr << "Engine Code:" << e.EngineCode() << endl;
				cerr << "Sql Code:" << e.SqlCode() << endl;

				throw string(e.what());
			}
		}
	}catch (IBPP::Exception& e)	{
		cout << e.what();
	}

	return id;
}

map<string,vector<rom> >& ModeloLocalRoms::roms2map(map<string,vector<rom> >& catalogo)
{
	try{
		__INIT_STATEMENT_START__

		st->Execute("select B.NOMBRE, lower(A.nombre), A.TAMANIO, A.CRC, A.SHA1 \
					from rom A,archivo B \
					where A.id_archivo = B.id_archivo;");

		string nombre_archivo;
		string nombre_anterior;
		vector<rom> lista_roms;

		while (st->Fetch()) {
			rom data_rom;

			st->Get(1, nombre_archivo);
			if (nombre_anterior.empty()) nombre_anterior = nombre_archivo;
			st->Get(2, data_rom.nombre);
			st->Get(3, (int32_t*)&data_rom.tamanio);
			st->Get(4, data_rom.crc);
			st->Get(5, data_rom.sha1);

			if (nombre_anterior == nombre_archivo)
				lista_roms.push_back(data_rom);
			else {
				catalogo.insert(make_pair(nombre_anterior, lista_roms));
				nombre_anterior = nombre_archivo;
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

list<propiedadesArchivoZip>& ModeloLocalRoms::archivos(const string& directorio, list<propiedadesArchivoZip>& listado)
{
	listado.clear();

	try{
		__INIT_STATEMENT_START__

		st->Execute("select NOMBRE, fecha, TAMANIO, SHA1 from archivo order by nombre;");

		while (st->Fetch()) {
			propiedadesArchivoZip info;

			IBPP::Timestamp ts;

			st->Get(1, info._nombre);
			st->Get(2, ts);
			st->Get(3, (int32_t*)&info._bytes);
			st->Get(4, info._crc);

			struct tm timeinfo = {0};

			timeinfo.tm_hour = ts.Hours();
			timeinfo.tm_mday = ts.Day();
			timeinfo.tm_min  = ts.Minutes();
			timeinfo.tm_mon  = ts.Month() - 1;
			timeinfo.tm_sec  = ts.Seconds();
			timeinfo.tm_year = ts.Year() - 1900;
			timeinfo.tm_isdst = -1;

			// cout << "Base:" << info._nombre << ": " << ts.Hours() << ":" << ts.Minutes() << ":" << ts.Seconds() << " - " << ts.Day() << "/" << ts.Month() << "/" << ts.Year() << endl;

			info._fecha=mktime(&timeinfo);

			// cout << "timeinfo:" << info._nombre << ": " << timeinfo.tm_hour << ":" << timeinfo.tm_min << ":" << timeinfo.tm_sec << " - " << timeinfo.tm_mday << "/" << timeinfo.tm_mon + 1 << "/" << timeinfo.tm_year + 1900 << endl;

			listado.push_back(info);
		}

	}catch (IBPP::SQLException& e)	{
		std::cerr << e.what();
	};
	return listado;
}

bool ModeloLocalRoms::existe_archivo(const string& nombre, uint64_t bytes, time_t fecha, const string& crc)
{
	bool existe=false;
	try{
		__INIT_STATEMENT_START__

		st->Prepare("select id_archivo from archivo \
					where nombre=? and tamanio=? and fecha=?;");

		st->Set(1, nombre);
		st->Set(2, static_cast<int32_t>(bytes));

		struct tm* timeinfo = {0};
		time_t timestamp=fecha;
		timeinfo = localtime(&timestamp);

		st->Set(3, IBPP::Timestamp(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec));

		try{
			st->Execute();

			if (st->Fetch())
				existe = true;

			tr->Commit();
		}catch (IBPP::SQLException& e)	{
			tr->Rollback();
			throw string(e.what());
		}
	}catch (IBPP::SQLException& e)	{
		throw e.what();
	}

	return existe;
}

void ModeloLocalRoms::sacar_roms(const string& nombre_archivo, const string& directorio, vector<rom>& contenido)
{
	contenido.clear();

	try{
		__INIT_STATEMENT_START__

		string consulta;

		consulta = "select C.nombre, C.tamanio, lower(C.crc), lower(C.sha1) \
				from directorio A, archivo B, rom C \
				where C.id_archivo = B.id_archivo \
				and B.id_directorio = A.id_directorio \
				and B.nombre = ?";

		if (!directorio.empty()) consulta += " and A.directorio = ?";

		st->Prepare(consulta);

		st->Set(1, nombre_archivo);
		if (!directorio.empty()) st->Set(2, directorio);

		st->Execute();

		while (st->Fetch()) {
			rom data_rom;

			st->Get(1, data_rom.nombre);
			st->Get(2, (int32_t*)&data_rom.tamanio);
			st->Get(3, data_rom.crc);
			st->Get(4, data_rom.sha1);

			contenido.push_back(data_rom);
		}

		st->TransactionPtr()->Commit();
	}catch (IBPP::Exception& e)	{
		std::cerr << e.what();
		throw ;
	};
}


void ModeloLocalRoms::borrar_archivo(const string& nombre, const string& directorio)
{
	try {
		__INIT_STATEMENT_START__

		st->Prepare("delete from archivo \
				where nombre=? and \
				id_directorio = (select id_directorio from directorio where directorio=?);");

		st->Set(1, nombre);
		st->Set(2, directorio);

		try{
			st->Execute();

			tr->Commit();
		}catch (IBPP::SQLException& e)	{
			tr->Rollback();
			throw string(e.what());
		}
	}catch (IBPP::SQLException& e)	{
		throw e.what();
	}
}

pair<string, string> ModeloLocalRoms::buscar_rom(const string& nombre_zip, const string& nombre, uint64_t tamanio, const string& crc, const string& sha1)
{
	string nombre_archivo;
	string nombre_rom;
	string consulta;

	try{
		__INIT_STATEMENT_START__

		int indice=1;

		consulta = "select directorio.directorio || archivo.nombre, rom.nombre from rom,archivo,directorio \
					where rom.id_archivo = archivo.id_archivo \
					and archivo.id_directorio = directorio.id_directorio ";

		if (!nombre_zip.empty()) consulta += "and archivo.nombre=? ";
		if (!nombre.empty()) consulta += "and rom.nombre=? ";
		if (tamanio != static_cast<uint64_t>(-1)) consulta += "and rom.tamanio=? ";
		if (!crc.empty()) consulta += "and lower(rom.crc)=lower(?) ";
		if (!sha1.empty()) consulta += "and lower(rom.sha1)=lower(?) ";

		st->Prepare(consulta);

		if (!nombre_zip.empty()) st->Set(indice++, nombre_zip);
		if (!nombre.empty()) st->Set(indice++, nombre);
		if (tamanio != static_cast<uint64_t>(-1)) st->Set(indice++, static_cast<int32_t>(tamanio));
		if (!crc.empty()) st->Set(indice++, crc);
		if (!sha1.empty()) st->Set(indice++, sha1);

		st->Execute();

		if (st->Fetch()) {
			st->Get(1, nombre_archivo);
			st->Get(2, nombre_rom);
		}

		st->TransactionPtr()->Commit();
	}catch (IBPP::SQLException& e)	{
		std::cerr << e.what();
	};

	return make_pair(nombre_archivo, nombre_rom);
}

void ModeloLocalRoms::catalogarArchivo(const string& nombre, const string& directorio)
{
	if (id_archivo(nombre, directorio)!=0) throw string("Archivo ya catalogado.");

	GuruUnzip::iterator itfinal;

	for (GuruUnzip::iterator it(directorio + nombre);it!=itfinal;++it) {

		rom _rom;

		_rom.nombre = it.nombreArchivoActual();
		_rom.tamanio = it->uncompressed_size;
		_rom.crc = to_lower_copy(GuruCRC::dword_a_string(it->crc));

		GuruBuffer buf(it->uncompressed_size);

		GuruUnzip::cargar(it, buf);
		if (buf.buf() == NULL)
			_rom.sha1 = "0000000000000000000000000000000000000000";
		else
			_rom.sha1 = to_lower_copy(GuruSHA1::sha1((byte*)buf.buf(), it->uncompressed_size));

		insertar_rom(nombre, _rom, directorio);
	}
}

bool ModeloLocalRoms::verificar_juego(const vector<rom>& roms_mame)
{
	string consulta;

	try{
		__INIT_STATEMENT_START__

		consulta = "select count(distinct(rom.sha1)) from rom,archivo,directorio \
					where rom.id_archivo = archivo.id_archivo \
					and archivo.id_directorio = directorio.id_directorio ";

		for (vector<rom>::const_iterator it=roms_mame.begin();it!=roms_mame.end();++it){
			ostringstream oss;
			oss << " and (rom.nombre='" << it->nombre << "' and rom.tamanio=" << it->tamanio << " and rom.crc='" << it->crc << "' and rom.sha1='" << it->sha1 << "')";

			consulta += oss.str();
		}

		st->Execute(consulta);

		int32_t total=0;
		if (st->Fetch()) st->Get(1, total);

		st->TransactionPtr()->Commit();

		if (total == static_cast<int32_t>(roms_mame.size())) return true;
	}catch (IBPP::SQLException& e)	{
		std::cerr << e.what();
	};

	return false;
}

/*
select count(rom.sha1) from rom,archivo,directorio
where rom.id_archivo = archivo.id_archivo and archivo.id_directorio = directorio.id_directorio (
or (rom.nombre='1346b.cpu-u25' and rom.tamanio=2048 and rom.crc='8e68533e' and rom.sha1='a257c556d31691068ed5c991f1fb2b51da4826db')
or (rom.nombre='5092.prom-u1' and rom.tamanio=2048 and rom.crc='29e10a81' and rom.sha1='c4b4e6c75bcf276e53f39a456d8d633c83dcf485')
or (rom.nombre='5093.prom-u2' and rom.tamanio=2048 and rom.crc='e1edc3df' and rom.sha1='4f593546bbb0f50850dc6286cb514af6831c27a7')
or (rom.nombre='5094.prom-u3' and rom.tamanio=2048 and rom.crc='995773bb' and rom.sha1='98dd826527853bc031edfb9a821778cc3e906150')
or (rom.nombre='5095.prom-u4' and rom.tamanio=2048 and rom.crc='f887f575' and rom.sha1='de96573a91b60b090b1f441f1410ecad63c9467c')
or (rom.nombre='5096.prom-u5' and rom.tamanio=2048 and rom.crc='5545241e' and rom.sha1='ee504ccaab469100137717341a1b461175ff792d')
or (rom.nombre='5097.prom-u6' and rom.tamanio=2048 and rom.crc='428edb54' and rom.sha1='4f3df6017068d939014a8f638f28e3228acb7add')
or (rom.nombre='5098.prom-u7' and rom.tamanio=2048 and rom.crc='5bcb9d63' and rom.sha1='c0c91bc9f75ad88a6e15c554a980d5c075725fe8')
or (rom.nombre='5099.prom-u8' and rom.tamanio=2048 and rom.crc='0ea24ba3' and rom.sha1='95a30c9b63ef1c346df0da71af3fdecd1a75cb8f')
or (rom.nombre='5100.prom-u9' and rom.tamanio=2048 and rom.crc='a79af131' and rom.sha1='0ba34130174e196015bc9b9c135c420209dfd524')
or (rom.nombre='5101.prom-u10' and rom.tamanio=2048 and rom.crc='8a1cdae0' and rom.sha1='f7c617f9bdb7818e6069a981d0c8820deade134c')
or (rom.nombre='5102.prom-u11' and rom.tamanio=2048 and rom.crc='70826a15' and rom.sha1='a86322d0e8a88534e9b78dcde42ae4c441276913')
or (rom.nombre='5103.prom-u12' and rom.tamanio=2048 and rom.crc='7f80c5b0' and rom.sha1='00748cd5fc7f75fdca194e748524d406c006296d')
or (rom.nombre='5104.prom-u13' and rom.tamanio=2048 and rom.crc='0140930e' and rom.sha1='f8ef894c46d3663bd89d2d817675a67075d3e0d6')
or (rom.nombre='5105.prom-u14' and rom.tamanio=2048 and rom.crc='17807a05' and rom.sha1='bd99f5beab0155f6e4d2fab2fa5f4e147c5730d5')
or (rom.nombre='5106.prom-u15' and rom.tamanio=2048 and rom.crc='c7cdfa9d' and rom.sha1='6ab7adc60ac7bb53a7175e8de51924008737c9ac')
or (rom.nombre='5107.prom-u16' and rom.tamanio=2048 and rom.crc='95f8a2e6' and rom.sha1='89c92e000b3e1630380db779370cf9f5b13e5719')
or (rom.nombre='5108.prom-u17' and rom.tamanio=2048 and rom.crc='d371cacd' and rom.sha1='8f2cdcc0b4e3b77e0958d257e37accefc5749cde')
or (rom.nombre='5109.prom-u18' and rom.tamanio=2048 and rom.crc='48a20617' and rom.sha1='5b4bc3beda0404ff0a61bb42751b87f71817f363')
or (rom.nombre='5110.prom-u19' and rom.tamanio=2048 and rom.crc='7d26111a' and rom.sha1='a6d3652ae606a5b75026e524c9d6aaa78300741e')
or (rom.nombre='5111.prom-u20' and rom.tamanio=2048 and rom.crc='a888e175' and rom.sha1='4c0af94441bf51dfc852372a5b90d0830df81363')
or (rom.nombre='epr-1286.sound-16' and rom.tamanio=2048 and rom.crc='fbe0d501' and rom.sha1='bfa277689790f835d8a43be4beee0581e1096bcc')
or (rom.nombre='6331.sound-u8' and rom.tamanio=32 and rom.crc='1d298cb0' and rom.sha1='bb0bb62365402543e3154b9a77be9c75010e6abc')
)

 */
