import paho.mqtt.client as mqtt
import psycopg2
HOST = "broker.hivemq.com"
PORT = 1883
KEEPALIVE = 60

SUB_TOPICS = {
    'public/alive': 'Time',
    'public/redled' : 'Red diod',
    'public/irled': 'IR diod',
    'public/hr': 'Heart rate',
    'public/sp': 'SpO2'
}

DATA_DICT = {}
for value in SUB_TOPICS.values():
    DATA_DICT[value] = 0


def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    for topic in SUB_TOPICS.keys():
        client.subscribe(topic)





class DB:
    def __init__(self, dbname = "iot", host="localhost", user="postgres", password="Z200120", port="5432"):
        self.connection = psycopg2.connect(f"dbname={dbname} user={user} host={host} password={password} port={port}")
        self.cursor = self.connection.cursor()

    def addRow(self, data:dict):
        print("Add row")
        sql_request = f"""INSERT INTO public.signals ("time", redled, irled, heartrate, spo2) VALUES ('{data['Time']}'::text, '{data['Red diod']}'::text, '{data['IR diod']}'::text, '{data['Heart rate']}'::text, '{data['SpO2']}'::text);COMMIT;"""
        self.cursor.execute(sql_request)
db = DB()

def on_message(client, userdata, msg):
    payload = msg.payload.decode()  # Основное значение, приходящее в сообщение, например, показатель температуры
    time, data = payload.split(":")
    topic = msg.topic
    param_name = SUB_TOPICS[topic]
    if param_name != 'Time':
        DATA_DICT[param_name] = data
        print(topic + " " + data)
    else:
        DATA_DICT['Time'] = time
        db.addRow(DATA_DICT)
        print(f"New message: {time}")

def main():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(HOST, PORT, KEEPALIVE)
    client.loop_forever()


if __name__ == "__main__":
    main()