FROM debian:stable-slim

SHELL ["/bin/bash", "-c"]

WORKDIR /root

# A too high maximum number of file descriptors (with the default value
