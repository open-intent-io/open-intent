-- set up redis --

docker pull redis
docker run --name some-redis -p 6379:6379 -d redis

-- install packages --

npm install

-- start the server --

nodejs app.js

enjoy!
