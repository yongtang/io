import time
import grpc
import concurrent.futures as futures
import dataset_pb2_grpc
import sys

class gRPCServicer(object):
  # missing associated documentation comment in .proto file
  pass

  def Dataset(self, request, context):
    # missing associated documentation comment in .proto file
    print("VVVVV")
    return request
    sys.exit(1)
    pass
    context.set_code(grpc.StatusCode.UNIMPLEMENTED)
    context.set_details('Method not implemented!')
    raise NotImplementedError('Method not implemented!')



def serve():
  server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
  dataset_pb2_grpc.add_gRPCServicer_to_server(
      gRPCServicer(), server)
  server.add_insecure_port('localhost:50051')
  server.start()

serve()

print("continue...")
for i in range(10000):
  time.sleep(5)
  print("in loop...")

