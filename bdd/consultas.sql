-- consutlas a la base de mame

-- mostrar las roms repetidas
select name,sha1, count(*)
from ROM
group by name,sha1
order by 3 DESC
-- HAVING count(*) > 1;

-- mostrar la lista de roms de un juego
select ROM.NAME, ROM.SHA1, ROM.TAMANIO
from ROM, GAME
WHERE ROM.ID_GAME = GAME.ID_GAME AND
        GAME.NAME_GAME = 'gng'

-- cantidad de archivos por un juego
select FIRST 20 GAME.DESCRIPTION, COUNT(*)
FROM ROM, GAME
WHERE ROM.ID_GAME = GAME.ID_GAME 
-- AND GAME.NAME_GAME = 'gng'
GROUP by GAME.DESCRIPTION
ORDER by 2 DESC
--------------------------------------
select B.nombre from rom A, archivo B
where A.ID_ARCHIVO = B.ID_ARCHIVO
    and A.NOMBRE = '1943.20'

select * from rom, archivo
where rom.ID_ARCHIVO = archivo.ID_ARCHIVO
    and archivo.NOMBRE = 'mslug5.zip'

select * from archivo
where nombre='mslug5.zip'


