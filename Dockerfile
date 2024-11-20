FROM ubuntu:22.04

RUN apt-get update

RUN apt-get -y install curl

RUN apt-get install nasm gcc qemu-system make -y

COPY ./boot /edOS/boot

COPY ./kernel /edOS/kernel

COPY ./stdlib /edOS/stdlib

COPY ./userspace /edOS/userspace

WORKDIR /edOS

CMD ["make", "run"]