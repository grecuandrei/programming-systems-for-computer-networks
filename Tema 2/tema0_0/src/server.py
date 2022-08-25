from flask import Flask, Response, request, json
import pymongo
from pymongo import MongoClient
from bson.objectid import ObjectId
from http import HTTPStatus
from datetime import datetime
from os import getenv

RESPONSE_CONTENT_TYPE = 'application/json'

ID_ = '_id'
ID = 'id'
COUNTRY_ID = 'idTara'
NAME = 'nume'
LAT = 'lat'
LONG = 'lon'
TIMESTAMP = 'timestamp'
CITY_ID = 'idOras'
VALUE = 'valoare'
FROM = 'from'
UNTIL = 'until'

COUNTRY_COLS = [NAME, LAT, LONG]
CITY_COLS = [COUNTRY_ID, NAME, LAT, LONG, ]
TEMP_COLS = [VALUE, CITY_ID]

server = Flask(__name__)

def get_db():
	port=int(getenv('MONGO_PORT'))
	user=getenv('MONGO_USER')
	passwd=getenv('MONGO_PASSWORD')
	db=getenv('MONGO_DATABASE')
	client = MongoClient(host='mongodb',
                         port=port, 
                         username=user, 
                         password=passwd,
                        authSource="admin")
	return client[db]

def reset_db():
	db = get_db()
	db.tari.drop()
	db.orase.drop()
	db.temperaturi.drop()
	init_db(db)

def init_db(db):
	db.tari.create_index(NAME, unique = True)
	db.orase.create_index([(COUNTRY_ID,pymongo.ASCENDING), (NAME,pymongo.ASCENDING)], unique= True)
	db.temperaturi.create_index([(CITY_ID,pymongo.ASCENDING), (TIMESTAMP,pymongo.ASCENDING)], unique= True)

def check_db():
	db = get_db()
	if "tari" in db.list_collection_names() and "orase" in db.list_collection_names() and "temperaturi" in db.list_collection_names():
		init_db(db)
		return True
	return False

# RESET THE DB
@server.route('/api/resetDB', methods=['DELETE'])
def ping_server():
	reset_db()
	return Response(status = 200)

# POST COUNTRY
@server.route('/api/countries', methods=['POST'])
def post_c():
	country_col = get_db().tari
	params = request.get_json(silent=True)
	if not params or params.get(NAME) is None or params.get(LAT) is None or params.get(LONG) is None:
		return Response(status=400)

	new_country = {
		NAME: params[NAME],
		LAT: params[LAT],
		LONG: params[LONG]
	}

	try:
		res = country_col.insert_one(new_country)
	except pymongo.errors.DuplicateKeyError:
		return Response(status=409)
	
	return Response(
		status=HTTPStatus.CREATED,
		response=json.dumps({ID: str(res.inserted_id)}),
		mimetype=RESPONSE_CONTENT_TYPE)

# GET COUNTRIES
@server.route('/api/countries', methods=['GET'])
def get_c():
	params = request.get_json(silent=True)
	if params:
		return Response(status=400)

	country_col = get_db().tari

	res = list(country_col.find())

	return_json = []
	for item in res:
		new_item = {
			ID: str(item[ID_]),
			NAME: item[NAME],
			LAT: item[LAT],
			LONG: item[LONG]
		}
		return_json.append(new_item)
	return Response(
		status=HTTPStatus.OK,
		response=json.dumps(return_json, indent=2),
		mimetype=RESPONSE_CONTENT_TYPE)

# UPDATE COUNTRY
@server.route('/api/countries/<id>', methods=['PUT'])
def update_c(id):
	params = request.get_json(silent=True)
	if not params or not ObjectId.is_valid(id):
		return Response(status=400)

	for key in params.keys():
		if key == ID:
			if not ObjectId.is_valid(params[ID]) or str(params[ID]) != str(id):
				return Response(status=400)
			continue
		if key == NAME:
			if not isinstance(params[key], str):
				return Response(status=400)
		if key == LAT or key == LONG:
			if isinstance(params[key], str):
				return Response(status=400)
		if key not in COUNTRY_COLS:
			return Response(status=400)

	country_col = get_db().tari

	query = {ID_: ObjectId(id)}

	if len(list(country_col.find(query))) == 0:
		return Response(status=404)

	new_vals = {
		"$set": {}
	}

	for key, value in params.items():
		if key != ID:
			new_vals["$set"][key] = value

	try :
		res = country_col.update_one(query, new_vals)
	except pymongo.errors.DuplicateKeyError:
		return Response(status=409)
	
	return Response(status=200)

# DELETE COUNTRY
@server.route('/api/countries/<id>', methods=['DELETE'])
def delete_c(id):
	if request.get_json(silent=True):
		return Response(status=400)

	if not ObjectId.is_valid(id):
		return Response(status=404)
	
	country_col = get_db().tari

	query = {ID_: ObjectId(id)}

	if len(list(country_col.find(query))) == 0:
		return Response(status=404)

	res = list(country_col.find(query))

	# get the cities
	queryOras = {COUNTRY_ID: res[0][ID_]}
	res = list(get_db().orase.find(queryOras))

	city_ids = []
	for item in res:
		city_ids.append(item[ID_])

	# get the temperatures
	queryTemp = {CITY_ID: {"$in":city_ids}}
	res = list(get_db().temperaturi.find(queryTemp))

	temps_ids = []
	for item in res:
		temps_ids.append(item[ID_])

	# DEL Temperatures based on a country
	queryTemp = {ID_: {"$in":temps_ids}}
	res = get_db().temperaturi.delete_many(queryTemp)

	# DEL Cities based on a country
	queryCity = {ID_: {"$in":city_ids}}
	res = get_db().orase.delete_many(queryCity)

	# DEL Country
	res = country_col.delete_one(query)
	
	return Response(status = 200)

# --------------------------------------------------------------------------------------

# POST CITY
@server.route('/api/cities', methods=['POST'])
def post_city():
	city_col = get_db().orase
	params = request.get_json(silent=True)
	if not params or params.get(COUNTRY_ID) is None or params.get(NAME) is None or params.get(LAT) is None or params.get(LONG) is None:
		return Response(status=400)
	
	if not ObjectId.is_valid(str(params[COUNTRY_ID])) or len(list(get_db().tari.find({ID_:ObjectId(str(params[COUNTRY_ID]))}))) == 0:
		return Response(status=400)

	new_city = {
		COUNTRY_ID: ObjectId(str(params[COUNTRY_ID])),
		NAME: params[NAME],
		LAT: params[LAT],
		LONG: params[LONG]
	}

	try:
		res = city_col.insert_one(new_city)
	except pymongo.errors.DuplicateKeyError:
		return Response(status=409)
	
	return Response(
		status=HTTPStatus.CREATED,
		response=json.dumps({ID: str(res.inserted_id)}),
		mimetype=RESPONSE_CONTENT_TYPE)

# GET CITIES
@server.route('/api/cities', methods=['GET'])
def get_cities():
	city_col = get_db().orase
	res = list(city_col.find())
	return_json = []
	for item in res:
		new_item = {
			ID: str(item[ID_]),
			COUNTRY_ID: str(item[COUNTRY_ID]),
			NAME: item[NAME],
			LAT: item[LAT],
			LONG: item[LONG]
		}
		return_json.append(new_item)
	return Response(
		status=HTTPStatus.OK,
		response=json.dumps(return_json, indent=2),
		mimetype=RESPONSE_CONTENT_TYPE)

# GET CITY
@server.route('/api/cities/country/<id_tara>', methods=['GET'])
def get_city(id_tara):
	if not ObjectId.is_valid(id_tara):
		return Response(status=400)

	city_col = get_db().orase

	query = {COUNTRY_ID: ObjectId(id_tara)}
	
	res = list(city_col.find(query))
	return_json = []
	for item in res:
		new_item = {
			ID: str(item[ID_]),
			COUNTRY_ID: str(item[COUNTRY_ID]),
			NAME: item[NAME],
			LAT: item[LAT],
			LONG: item[LONG]
		}
		return_json.append(new_item)
	return Response(
		status=HTTPStatus.OK,
		response=json.dumps(return_json, indent=2),
		mimetype=RESPONSE_CONTENT_TYPE)

# UPDATE CITY
@server.route('/api/cities/<id>', methods=['PUT'])
def update_city(id):
	params = request.get_json(silent=True)
	
	if not params:
		return Response(status=400)

	if not ObjectId.is_valid(id):
		return Response(status=404)

	for key in params.keys():
		if key == ID:
			if not ObjectId.is_valid(params[ID]) or str(params[ID]) != str(id):
				return Response(status=400)
			continue
		if key == COUNTRY_ID:
			if not ObjectId.is_valid(str(params[key])) or len(list(get_db().tari.find({ID_:ObjectId(str(params[key]))}))) == 0:
				return Response(status=400)
		if key == NAME:
			if not isinstance(params[key], str):
				return Response(status=400)
		if key == LAT or key == LONG:
			if isinstance(params[key], str):
				return Response(status=400)
		if key not in CITY_COLS:
			return Response(status=400)
	
	city_col = get_db().orase

	query = {ID_: ObjectId(id)}

	if len(list(city_col.find(query))) == 0:
		return Response(status=404)

	new_vals = {
		"$set": {}
	}

	for key, value in params.items():
		if key != ID:
			new_vals["$set"][key] = value

	try:
		res = city_col.update_one(query, new_vals)
	except pymongo.errors.DuplicateKeyError:
		return Response(status=409)
	
	return Response(status=200)

# DELETE CITY
@server.route('/api/cities/<id>', methods=['DELETE'])
def delete_city(id):
	if request.get_json(silent=True):
		return Response(status=400)

	if not ObjectId.is_valid(id):
		return Response(status=404)
	
	city_col = get_db().orase

	query = {ID_: ObjectId(id)}

	if len(list(city_col.find(query))) == 0:
		return Response(status=404)

	# get the temperatures
	queryCity = {CITY_ID: ObjectId(id)}
	res = list(get_db().temperaturi.find(queryCity))

	temps_ids = []
	for item in res:
		temps_ids.append(item[ID_])

	# DEL Temperatures based on a country
	queryTemp = {ID_: {"$in":temps_ids}}
	res = get_db().temperaturi.delete_many(queryTemp)

	# DEL City
	res = city_col.delete_one(query)
	
	return Response(status = 200)

# --------------------------------------------------------------------------------------

# POST TEMP
@server.route('/api/temperatures', methods=['POST'])
def post_t():
	params = request.get_json(silent=True)
	if not params or params.get(CITY_ID) is None or params.get(VALUE) is None or isinstance(params.get(VALUE), str):
		return Response(status=400)
	
	if not ObjectId.is_valid(str(params[CITY_ID])) or len(list(get_db().orase.find({ID_:ObjectId(str(params[CITY_ID]))}))) == 0:
		return Response(status=400)

	temp_col = get_db().temperaturi

	time = datetime.now().isoformat()

	new_temp = {
		VALUE: params[VALUE],
		TIMESTAMP: time,
		CITY_ID: ObjectId(str(params[CITY_ID]))
	}

	try:
		res = temp_col.insert_one(new_temp)
	except pymongo.errors.DuplicateKeyError:
		return Response(status=409)
	
	return Response(
		status=HTTPStatus.CREATED,
		response=json.dumps({ID: str(res.inserted_id)}),
		mimetype=RESPONSE_CONTENT_TYPE)

# GET TEMP
@server.route('/api/temperatures', methods=['GET'])
def get_t():
	queryOras = {}

	for k, v in request.args.items():
		if k == LAT or k == LONG:
			queryOras[k] = float(v)

	if bool(queryOras):
		city_col = get_db().orase

		res = list(city_col.find(queryOras))
	
		city_id = []
		for item in res:
			city_id.append(item[ID_])

		queryTemp = {CITY_ID: {"$in":city_id}}
	else :
		queryTemp = {}

	for k in request.args.keys():
		if k == FROM or k == UNTIL:
			queryTemp[TIMESTAMP] = {}
			break

	for k, v in request.args.items():
		if k == FROM:
			date = v.split("-")
			start = datetime(int(date[0]), int(date[1]), int(date[2])).isoformat()
			queryTemp[TIMESTAMP]['$gte'] = start
		elif k == UNTIL:
			date = v.split("-")
			end = datetime(int(date[0]), int(date[1]), int(date[2])).isoformat()
			queryTemp[TIMESTAMP]['$lt'] = end

	temp_col = get_db().temperaturi

	res = list(temp_col.find(queryTemp))
	return_json = []
	for item in res:
		new_item = {
			ID: str(item[ID_]),
			VALUE: item[VALUE],
			TIMESTAMP: item[TIMESTAMP][0:10]
		}
		return_json.append(new_item)
	return Response(
		status=HTTPStatus.OK,
		response=json.dumps(return_json, indent=2),
		mimetype=RESPONSE_CONTENT_TYPE)

# GET TEMP CITY
@server.route('/api/temperatures/cities/<id>', methods=['GET'])
def get_tcity(id):
	if not ObjectId.is_valid(id):
		return Response(status=400)

	queryTemp = {CITY_ID: ObjectId(str(id))}

	for k in request.args.keys():
		if k == FROM or k == UNTIL:
			queryTemp[TIMESTAMP] = {}
			break

	for k, v in request.args.items():
		if k == FROM:
			date = v.split("-")
			start = datetime(int(date[0]), int(date[1]), int(date[2])).isoformat()
			queryTemp[TIMESTAMP]['$gte'] = start
		elif k == UNTIL:
			date = v.split("-")
			end = datetime(int(date[0]), int(date[1]), int(date[2])).isoformat()
			queryTemp[TIMESTAMP]['$lt'] = end

	temp_col = get_db().temperaturi

	res = list(temp_col.find(queryTemp))
	return_json = []
	for item in res:
		new_item = {
			ID: str(item[ID_]),
			VALUE: item[VALUE],
			TIMESTAMP: item[TIMESTAMP][0:10]
		}
		return_json.append(new_item)

	return Response(
		status=HTTPStatus.OK,
		response=json.dumps(return_json, indent=2),
		mimetype=RESPONSE_CONTENT_TYPE)

# GET TEMP COUNTRY
@server.route('/api/temperatures/countries/<id>', methods=['GET'])
def get_tcountry(id):
	if not ObjectId.is_valid(id):
		return Response(status=400)

	queryTara = {ID_: ObjectId(str(id))}

	res = list(get_db().tari.find(queryTara))

	queryOras = {COUNTRY_ID: res[0][ID_]}

	res = list(get_db().orase.find(queryOras))

	city_id = []
	for item in res:
		city_id.append(item[ID_])

	queryTemp = {CITY_ID: {"$in":city_id}}

	for k in request.args.keys():
		if k == FROM or k == UNTIL:
			queryTemp[TIMESTAMP] = {}
			break

	for k, v in request.args.items():
		if k == FROM:
			date = v.split("-")
			start = datetime(int(date[0]), int(date[1]), int(date[2])).isoformat()
			queryTemp[TIMESTAMP]['$gte'] = start
		elif k == UNTIL:
			date = v.split("-")
			end = datetime(int(date[0]), int(date[1]), int(date[2])).isoformat()
			queryTemp[TIMESTAMP]['$lt'] = end

	temp_col = get_db().temperaturi

	res = list(temp_col.find(queryTemp))
	return_json = []
	for item in res:
		new_item = {
			ID: str(item[ID_]),
			VALUE: item[VALUE],
			TIMESTAMP: item[TIMESTAMP][0:10]
		}
		return_json.append(new_item)

	return Response(
		status=HTTPStatus.OK,
		response=json.dumps(return_json, indent=2),
		mimetype=RESPONSE_CONTENT_TYPE)

# UPDATE TEMP
@server.route('/api/temperatures/<id>', methods=['PUT'])
def update_t(id):
	params = request.get_json(silent=True)

	if not params or not ObjectId.is_valid(id):
		return Response(status=400)

	for key in params.keys():
		if key == ID:
			if not ObjectId.is_valid(params[ID]) or str(params[ID]) != str(id):
				return Response(status=400)
			continue
		if key == CITY_ID:
			if not ObjectId.is_valid(str(params[key])) or len(list(get_db().orase.find({ID_:ObjectId(str(params[key]))}))) == 0:
				return Response(status=400)
		if key == VALUE:
			if isinstance(params[key], str):
				return Response(status=400)
		if key not in TEMP_COLS:
			return Response(status=400)
	
	temp_col = get_db().temperaturi

	query = {ID_: ObjectId(id)}

	if len(list(temp_col.find(query))) == 0:
		return Response(status=404)

	new_vals = {
		"$set": {}
	}

	for key, value in params.items():
		new_vals["$set"][key] = value

	time = datetime.now().isoformat()
	new_vals["$set"][TIMESTAMP] = time

	try:
		res = temp_col.update_one(query, new_vals)
	except pymongo.errors.DuplicateKeyError:
		return Response(status=409)
	
	return Response(status=200)

# DELETE TEMP
@server.route('/api/temperatures/<id>', methods=['DELETE'])
def delete_t(id):
	if request.get_json(silent=True):
		return Response(status=400)

	if not ObjectId.is_valid(id):
		return Response(status=404)
	
	temp_col = get_db().temperaturi

	query = {ID_: ObjectId(id)}

	if len(list(temp_col.find(query))) == 0:
		return Response(status=404)

	# DEL temperature
	res = temp_col.delete_one(query)
	
	return Response(status = 200)

# --------------------------------------------------------------------------------------

if __name__ == "__main__":
	if not check_db():
		reset_db()
	server.run('0.0.0.0', port=5000)