FROM asneg/opcuastack:release-4

MAINTAINER Aleksey Timin <atimin@gmail.com>

# Prepare workdir
ADD / /tmp
RUN mkdir /build
WORKDIR /build

# Build
RUN cmake /tmp/src
RUN make
RUN make install

# Start DEMO
EXPOSE 8889 4840
CMD ["OpcUaServer4", "/etc/OpcUaStack/ASNeG-Demo/OpcUaServer.xml"]
