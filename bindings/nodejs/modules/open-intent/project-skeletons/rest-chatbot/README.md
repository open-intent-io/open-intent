* General

This is an example of REST chatbot with a debugging CLI. You can use it to build up your own bot model with the open-intent markup language.

The program runs in a docker container and is monitoring the 3 model files 'script.txt', 'user_commands.js' and 'dictionary.json'.
Just put those files in a directory and mount it in /usr/src/res in the docker container to use your own model. You can even modify them at runtime and the bot will consume them right away.


* Run the example

Install the nodejs dependencies with:

    npm install

The following line will build the docker image

    docker build -t chatbot .

The following line will start your chatbot service on port 3000 with the ability to test as a CLI program

    docker run --rm -e SERVICE_PORT=3000 -e DEBUG=true -v $(pwd)/custom_res:/usr/src/res -it -p 3000:3000 chatbot

If you don't need the CLI but only the REST API just remove the DEBUG env variable and the interactive terminal from the above command line 

    docker run --rm -e SERVICE_PORT=3000 -v $(pwd)/custom_res:/usr/src/res -p 3000:3000 chatbot
