from datetime import datetime
import json
import jsonlines
import os
import random

#JSON data structure:
"""
Main data:
	time:			String
	redled:	        int
	irled: 		    int
	heartRate:		int
	spo2:			int
"""




def generator_params():
    parameters = {
        'time':datetime.strftime(datetime.now(), '%H:%M:%S %d.%m.%Y'),
        'redled': random.randint(0, 16545),
        'irled': random.randint(0, 16545),
        'heartRate': random.randint(0, 1000),
        'spo2': random.randint(0, 1000),
    }
    return parameters

def createJSON(data, path='data.json'):
    with open('data.json', 'w') as file:
        json_string = json.dumps(data)
        file.write(json_string)
