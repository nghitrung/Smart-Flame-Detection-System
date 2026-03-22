FROM node:18-alpine

RUN npm install -g nodemon

WORKDIR /usr/src/app

COPY package*.json ./

RUN npm install 

COPY . .

EXPOSE 5000

CMD ["nodemon", "src/app.js"]