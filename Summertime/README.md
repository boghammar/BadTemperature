# Summertime


## Installation
Push to private docker registry
```
docker tag summertime boghammar/privatebuilds
docker push boghammar/privatebuilds:latest
```

Run as a docker container. It expose port 3000 so map that to the port you want to expose on the host
```
docker pull boghammar/privatebuilds:latest
create .env
docker run -dp 8080:3000 -h=`hostname` --name summertime --env-file .env boghammar/privatebuilds:latest
```
