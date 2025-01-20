FROM node:23-alpine
#FROM node:latest
#RUN apk add --no-cache python3 g++ make
WORKDIR /Summertime
COPY . .
RUN npm install 
# final config
EXPOSE 3000
VOLUME /Config
CMD ["node", "./bin/www"]