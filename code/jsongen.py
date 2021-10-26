from datetime import datetime
import json
import jsonlines
import os
import random

#JSON data structure:
"""
Main data:
	time:			timestamp
	redSignal:		int
	greenSignal:	int
	IRSignal: 		int
	heartRate:		int
	validHeartRate:	int
	spo2:			int
	validSPO2:		int

Config:
	ledBrightness:	byte
	sampleAverage: 	byte
	ledMode:		byte
	sampleRate:		byte
	pulseWidth:		int
	adcRange:		int
"""


def generator_params():
    parameters = {
        'time':datetime.strftime(datetime.now(), '%H:%M:%S %d.%m.%Y'),
        'redSignal': random.randint(0, 16545),
        'greenSignal': random.randint(0, 16545),
        'IRSignal': random.randint(0, 16545),
        'heartRate': random.randint(0, 1000),
        'validHeartRate': random.randint(0, 200),
        'spo2': random.randint(0, 1000),
        'validSPO2': random.randint(0, 200),
        'ledBrightness': 60,
        'sampleAverage': 4,
        'ledMode': 2,
        'sampleRate':100,
        'pulseWidth': 411,
        'adcRange': 4096
    }
    return parameters

def createJSON(data, path='data.json'):
    with open('data.json', 'w') as file:
        json_string = json.dumps(data)
        file.write(json_string)
