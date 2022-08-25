import requests
import json

def ex1(nume, grupa):
    print('-- Ex 1 --')

    # url
    url = 'https://sprc.dfilip.xyz/lab1/task1'

    # create args for requests
    params = {'nume': nume, 'grupa': grupa}
    data = {'secret': 'SPRCisNice'}
    headers = {'secret2': 'SPRCisBest'}
    
    # post
    requests.post(url, params = params, data = data, headers = headers)
    
    # post request at url_check
    url_check = 'https://sprc.dfilip.xyz/lab1/task1/check' 
    response = requests.post(url_check, params = params, data = data, headers = headers)

    return response.json()

def ex2(nume):
    print('-- Ex 2 --')
    # url
    url = 'https://sprc.dfilip.xyz/lab1/task2'

    # payload
    payload = {
        'username': 'sprc',
        'password': 'admin',
        'nume': nume
    }

    response = requests.post(url, json = payload)
    return response.json()

def ex3(nume):
    print('-- Ex 3 --')

    # login payload
    url = 'https://sprc.dfilip.xyz/lab1/task3/login'
    payload = {
        'username': 'sprc',
        'password': 'admin',
        'nume': nume
    }
    
    # create session
    session = requests.Session()

    # post
    session.post(url, json = payload)

    # get
    url_get = 'https://sprc.dfilip.xyz/lab1/task3/check'
    return session.get(url_get).json()

if __name__ == "__main__":
    nume = input('Nume: ') 
    print(nume)
    grupa = input('Grupa: ')
    print(grupa)
    print(ex1(nume, grupa))
    print(ex2(nume))
    print(ex3(nume))