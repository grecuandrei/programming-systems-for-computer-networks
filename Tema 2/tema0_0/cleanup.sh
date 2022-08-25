#! /bin/sh

docker-compose down -v
docker-compose rm

docker rmi mongo:latest mongo-express:latest python:3.8 server
