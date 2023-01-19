#FROM --platform=linux/amd64 ubuntu:20.04 as builder
FROM fuzzers/libfuzzer:12.0 as builder

#RUN apt-get update && \
#    DEBIAN_FRONTEND=noninteractive apt-get install -y cmake clang
    
ADD . /c-vector
WORKDIR /c-vector/mayhem

#RUN mkdir -p build
#WORKDIR build
#RUN CC=clang CXX=clang++ cmake ..
#RUN make

RUN clang-12 -fsanitize=fuzzer,address fuzzer.c -o c-vector-fuzzer 

#FROM --platform=linux/amd64 ubuntu:20.04

FROM fuzzers/libfuzzer:12.0

COPY --from=builder /c-vector/mayhem/c-vector-fuzzer /

#COPY --from=builder /c-vector/build/mayhem/c-vector-fuzzer /
#COPY --from=builder /c-vector/build/mayhem/c-vector-fuzzer-memory /
#COPY --from=builder /c-vector/build/mayhem/c-vector-fuzzer-address /

ENTRYPOINT []
CMD /c-vector-fuzzer
