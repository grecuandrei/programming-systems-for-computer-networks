# Tema 2 - REST API & Docker

## Rulare

```
docker-compose up --build
```

## Structura
### API
Serverul web ce expune *API*-ul este implementat in *Python* folosind biblioteca
`Flask` si `PyMongo`, deoarece aceasta combinatie de limbaj si tehnologie imi este cea mai
familiara.

Fisierele sursa ale serverul se afla, impreuna cu fisierele `Dockerfile` si
`requirements.txt` in directorul `src/`.

Pentru a se conecta la baza de date, *Mongo-Express* are nevoie de restartarea automata in
caz de fail (porneste mai repede ca baza) si de aceea exista posibilitatea ca,
comanda *docker-compose up* sa nu elibereze complet terminalul
atunci cand se apeleaza *docker-compose down* (=> trebuie o iesire fortata: CTRL+C)

*API*-ul ruleaza pe portul `5000`.

Pentru a RESETA baza de date in cazul unor erori am implementat o noua ruta `/api/resetDB`
de tipul *DELETE*.

### Baza de date
Aceasta este de tip *MongoDB* pentru ca imi este cunoscuta si implementarea
a fost mai usoara. De asemenea ofera posibilitatea de a crea constrangeri de tipul
unique, iar accesul la o baza de date/colectie este foarte usoara.

Pentru conectare, se vor folosi credentialele (mentionate in fisierul
`global.variables.env`):
```
username: meteoadmin
password: meteo
```

Baza de date poate fi accesata de pe `localhost` prin portul `27017`.

### Utilitarul de gestiune
Utilitarul folosit este
[mongo-express](https://hub.docker.com/_/mongo-express), dedicat pentru *MongoDB*.

*Mongo-express* expune portul `8081` si poate accesa baza de date pe portul
`27017`.

## Stergere
Scriptul `cleanup.sh` disponibil in radacina proiectului sterge din sistem toate
resursele descarcate sau create (fisiere, volume, retele) de aplicatie.

## Testare
Teste pentru *Postman* cu care a fost verificata aplicatia se gaseste
in fisierul `tests/TestAPI-Tema2-v2.json`.