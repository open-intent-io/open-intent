#!/bin/bash

cd bindings/nodejs

version=`npm version patch --git-tag-version -m"Update npm version to %s"`

git commit -m"npm version $version" package.json

npm publish
