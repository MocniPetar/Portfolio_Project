FROM ubuntu:25.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && \
    apt install -y \
        libcurl4-openssl-dev \
        git \
        nano \
        build-essential

EXPOSE 8080

COPY ./home /home/ubuntu

WORKDIR /home/ubuntu/Server

CMD ["make"]