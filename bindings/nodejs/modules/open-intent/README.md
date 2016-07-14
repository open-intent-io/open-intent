# open-intent

open-intent npm module allows you to build chatbot in seconds with your own model.

# Quick start
Install open-intent module, create a chatbot project and start it with the following commands

    1. npm install -g open-intent

    2. open-intent create mychatbot

    3. open-intent start mychatbot

The command #2 creates a directory called mychatbot/ with a skeleton of nodejs application containing the REST chatbot. The model files you're gonna be editing are located under mychatbot/res/.

The command #3 starts your project with a monitor that is watching your model files. If one of them is edited while the chatbot is running, it will restart automatically to take the new model into account. This is intended for prototyping only.

For more explanation on the model, please refer to the [documentation](https://github.com/open-intent-io/open-intent/wiki).