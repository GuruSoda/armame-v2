set heading off;

select 'insert into directorio (id_directorio, directorio) values (' || id_directorio || ',''' || directorio ||''');' from directorio;

select 'insert into archivo (id_archivo, nombre, fecha, tamanio, sha1, id_directorio) values (' || id_archivo || ',''' || nombre ||''','''||fecha ||''','||tamanio||','''||sha1||''','||id_directorio||');' from archivo;

select 'insert into rom (id_rom, id_archivo, nombre, tamanio, crc, sha1) values (' || id_rom || ',' || id_archivo ||','''||nombre ||''','||tamanio||','''||crc||''','''||sha1||''');' from rom;

select 'alter sequence seq_id_directorio restart with ' || next value for seq_id_directorio ||';' from RDB$DATABASE;

select 'alter sequence seq_id_archivo restart with ' || next value for seq_id_archivo ||';' from rdb$database;

select 'alter sequence seq_id_rom restart with ' || next value for seq_id_rom ||';' from rdb$database;

