FROM python:3.8

COPY requirements.txt /tmp
RUN apt-get update && apt-get install -y python3 python3-pip
RUN apt-get -y install python3.8-dev
RUN pip3 install --upgrade pip
RUN pip3 install -r /tmp/requirements.txt

ADD . /server/
WORKDIR /server

