FROM ubuntu:22.04

RUN apt update && \
    apt install -y cmake vim sudo

COPY scripts/install_depends.sh /install_depends.sh
RUN bash install_depends.sh
