import grpc
import data_pb2, data_pb2_grpc
import google.protobuf

_HOST = 'localhost'
_PORT = '8080'

def run():
    conn = grpc.insecure_channel(_HOST + ':' + _PORT)
    client = data_pb2_grpc.FormatDataStub(channel=conn)
    response = client.DoFormat2(data_pb2.Data(text='hello,world!'))
    response2 = data_pb2.Data()
    print("received: " + response2.text)
    response.Unpack(response2)
    print("received: " + response2.text)
    print("received: " + response.text)

if __name__ == '__main__':
    run()
