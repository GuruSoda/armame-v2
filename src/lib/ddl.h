#ifndef __DDL_MODELO_MAME_H__
#define __DDL_MODELO_MAME_H__

#define DDL const char* ddl_modelo_mame[] = {"\
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
    FOREIGN KEY(id_clonof) REFERENCES GAME(id_game), \
    FOREIGN KEY(id_manufacturer) REFERENCES MANUFACTURER(id_manufacturer) \
)\
","\
CREATE TABLE ROM_GAME( \
    id_game     INTEGER NOT NULL, \
    id_rom      INTEGER NOT NULL, \
    combinado   CHAR(1) DEFAULT 'N', \
\
    FOREIGN KEY(id_game) REFERENCES GAME(id_game), \
    FOREIGN KEY(id_rom) REFERENCES ROM(id_rom) \
)\
","\
CREATE TABLE ULTIMO_GAME ( \
    id_game		INTEGER \
)\
","\
CREATE TABLE ULTIMO_ROM ( \
    id_rom      INTEGER \
)\
","\
CREATE GENERATOR GEN_ID_ROM; \
","\
CREATE GENERATOR GEN_ID_MANUFACTURER;\
","\
CREATE GENERATOR GEN_ID_GAME; \
","\
CREATE EXCEPTION EXISTE_ROM 'ROM Existe'; \
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
CREATE TRIGGER AI_ROM FOR ROM \
ACTIVE AFTER INSERT \
POSITION 0 \
AS \
BEGIN \
    if (new.ID_ROM <> 0) then \
		insert into ULTIMO_ROM(id_rom) values (new.ID_ROM); \
END \
","\
CREATE TRIGGER AI_GAME FOR GAME \
ACTIVE AFTER INSERT \
POSITION 0 \
AS \
BEGIN \
    IF (NEW.ID_GAME <> 0) THEN \
        insert into ULTIMO_GAME(id_game) values (new.ID_GAME); \
END \
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

#endif
