FROM ubuntu:25.10

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && \
    apt install -y \
        libcurl4-openssl-dev \
        git \
        nano \
        build-essential

EXPOSE 8080

COPY ./home /home/ubuntu

WORKDIR /home/ubuntu/server

CMD ["make"]