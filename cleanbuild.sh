docker run --rm -it -v ${PWD}:/sources --user $(id -u):$(id -g) infinitime-build /opt/build.sh clean
docker run --rm -it -v ${PWD}:/sources --user $(id -u):$(id -g) infinitime-build
