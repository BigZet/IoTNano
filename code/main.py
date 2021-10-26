import paho.mqtt.client as mqtt
import jsongen
import jsonparser




def main():
    print(f"Create  random data")
    data = jsongen.generator_params()
    for j in data.keys():
         print(f"{j} = {data[j]}")
    path = 'data.json'
    print(f"Create or append json data file {path}")
    jsongen.createJSON(data=data, path=path)
    print()
    print("parsing")
    dataparsed = jsonparser.parser(path)
    for i in dataparsed:
        print(f"{i} = {dataparsed[i]}")
    print("End data generation")



if __name__ == '__main__':
    main()