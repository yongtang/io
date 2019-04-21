import grpc
import dataset_pb2_grpc
import dataset_pb2
import google.protobuf

returned = google.protobuf.any_pb2.Any()

channel = grpc.insecure_channel('localhost:50051')
stub = dataset_pb2_grpc.gRPCStub(channel)
v = stub.Dataset(dataset_pb2.DatasetRequest(mess="xyz"), 5)


foo = dataset_pb2.DatasetRequest()
returned.Unpack(foo)
print(v)
print(foo.mess)

