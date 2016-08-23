FROM open-intent
MAINTAINER open-intent.io

RUN npm install -g mocha

ADD entrypoint.sh /entrypoint.sh
ADD res/ /custom_res
ADD test/ /test

WORKDIR /usr/src

CMD ["/entrypoint.sh"]

