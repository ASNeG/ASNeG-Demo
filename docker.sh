
RELEASE="release-4"

docker image rm asneg/asneg-demo:${RELEASE}
docker image build -t asneg/asneg-demo:${RELEASE} .
docker image push asneg/asneg-demo:${RELEASE}
docker image rm asneg/asneg-demo:${RELEASE}
