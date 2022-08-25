# Laboratot 6 - Web Cache LFU

Pentru a implementa *LFU*, am adugat clasei `WebCacheEntry` un nou
camp, `frequency`, care se incrementeaza la fiecare accesare a paginii.
Astfel, atunci cand este nevoie sa se elimine o pagina din cache, se va
alege cea cu valoarea minima in acest camp.

## Testare
Rularea nu se opreste decat la inchiderea executiei.

Trebuie lasat ceva timp pentru a se observa ceva relevant.

## Output
In outputul din rularea personala, `out.txt`, s-a cautat un snippet relevant:

Size On Disk vs Max Size On Disk
310969 vs 204800
REMOVED because space: http://google.com from cache with freq:1
REMOVED because space: http://romania-durabila.gov.ro/ from cache with freq:1
REMOVED because space: http://www.ms.ro/ from cache with freq:1
REMOVED because space: http://httpforever.com from cache with freq:4
ACCESSED: http://www.ms.ro/ got accesed for 1 times
ACCESSED: http://www.romania-actualitati.ro/ accesed for 4 times
ACCESSED: http://www.yahoo.com accesed for 5 times
REMOVED because time: http://www.yahoo.com from cache with freq:5

Se poate observa ca aceast cod se comporta asa cum ar trebui, se elimina intai
paginile accesate de mai putine ori (frecventa = 1) si apoi se elimina
http://httpforever.com cu 4 accesari (frecventa = 4).

