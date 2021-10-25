import json
import random
import paho.mqtt.client as mqtt
from datetime import datetime


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
    with open(path, 'a') as file:
        json_string = json.dumps(data)  # Формирование строки JSON из словаря
        file.write(json_string + ',\n')

def main():
    for i in range(100):
        print(f"Create #{i} random data")
        data = generator_params()
        for j in data.keys():
            print(f"{j} = {data[j]}")
        path = 'data.json'
        print(f"Create or append json data file {path}")
        createJSON(data=data, path=path)
        print()
    print("End data generation")



if __name__ == '__main__':
    main()