import json


def parser(path):
    data=[]
    with open('data.json', 'r') as file:
       data = json.load(file)
    return data