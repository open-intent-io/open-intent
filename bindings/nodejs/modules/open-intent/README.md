# open-intent

open-intent npm module allows you to build chatbot in seconds with your own model.

# Quick start
Install open-intent module, create a chatbot project and start it with the following commands

    npm install -g open-intent

    open-intent project create mychatbot

    open-intent project start mychatbot

The command #2 creates a directory called mychatbot/ with a skeleton of nodejs application containing the REST chatbot. The model files you're gonna be editing are located under mychatbot/res/.

The command #3 starts your project with a monitor that is watching your model files. If one of them is edited while the chatbot is running, it will restart automatically to take the new model into account. This is intended for prototyping only.

For more explanation on the model, please refer to the [documentation](https://github.com/open-intent-io/open-intent/wiki).

# REST API

The newly created chatbot uses swagger as REST API. You can see the documentation at [http://localhost:10010/doc](http://localhost:8080/docs)